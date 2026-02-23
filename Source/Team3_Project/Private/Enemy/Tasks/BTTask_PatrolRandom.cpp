#include "Enemy/Tasks/BTTask_PatrolRandom.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_PatrolRandom::UBTTask_PatrolRandom()
{
    NodeName = "Patrol Random";
    bNotifyTick = true;

    PatrolLocationKey.SelectedKeyName = FName("PatrolLocation");
    HomeLocationKey.SelectedKeyName = FName("HomeLocation");

    CurrentState = EPatrolState::MovingToLocation;
}

EBTNodeResult::Type UBTTask_PatrolRandom::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    // ========================================
    // 순찰 속도 설정
    // ========================================
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetPatrolSpeed();
    }

    // ========================================
    // HomeLocation 가져오기
    // ========================================
    FVector HomeLocation = BB->GetValueAsVector(HomeLocationKey.SelectedKeyName);
    if (HomeLocation.IsZero())
    {
        HomeLocation = Enemy->GetActorLocation();
        BB->SetValueAsVector(HomeLocationKey.SelectedKeyName, HomeLocation);
    }

    // ========================================
    // 랜덤 위치 찾기
    // ========================================
    const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(AIController->GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Patrol Random: No NavSystem"));
        return EBTNodeResult::Failed;
    }

    FNavLocation RandomLocation;
    float PatrolRadius = Enemy->GetPatrolRadius();

    bool bFound = NavSystem->GetRandomReachablePointInRadius(
        HomeLocation,
        PatrolRadius,
        RandomLocation
    );

    if (!bFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BT] Patrol Random: Could not find random location"));
        return EBTNodeResult::Failed;
    }

    // ========================================
    // 이동 시작
    // ========================================
    UE_LOG(LogTemp, Log, TEXT("[BT] Patrol to: %s"), *RandomLocation.Location.ToString());

    BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);

    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
        RandomLocation.Location,
        AcceptanceRadius,
        true,  // bStopOnOverlap
        true,  // bUsePathfinding
        false, // bProjectDestinationToNavigation
        true,  // bCanStrafe
        nullptr,
        true   // bAllowPartialPath
    );

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Patrol Random: MoveToLocation failed"));
        return EBTNodeResult::Failed;
    }

    CurrentState = EPatrolState::MovingToLocation;
    return EBTNodeResult::InProgress;
}

void UBTTask_PatrolRandom::TickTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    switch (CurrentState)
    {
        case EPatrolState::MovingToLocation:
        {
            EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

            if (Status == EPathFollowingStatus::Idle)
            {
                // ✅ 도착했는지 확인 (Idle이어도 실패일 수 있음)
                UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
                if (PathComp && PathComp->DidMoveReachGoal())
                {
                    // 성공적으로 도착
                    UE_LOG(LogTemp, Log, TEXT("[BT] Patrol: Arrived, starting wait"));

                    AIController->StopMovement();

                    WaitDuration = FMath::RandRange(WaitTimeMin, WaitTimeMax);
                    WaitStartTime = AIController->GetWorld()->GetTimeSeconds();
                    CurrentState = EPatrolState::Waiting;
                }
                else
                {
                    // 실패
                    UE_LOG(LogTemp, Warning, TEXT("[BT] Patrol: Failed to reach goal"));
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                }
            }
        }
            break;
        case EPatrolState::Waiting:
        {
            float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
            float ElapsedTime = CurrentTime - WaitStartTime;

            if (ElapsedTime >= WaitDuration)
            {
                UE_LOG(LogTemp, Log, TEXT("[BT] Patrol: Wait finished"));
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
        }
        break;
    }
}