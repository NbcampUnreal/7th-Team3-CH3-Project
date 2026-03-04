#include "Enemy/Tasks/BTTask_PatrolRandom.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "NavFilters/NavigationQueryFilter.h"

UBTTask_PatrolRandom::UBTTask_PatrolRandom()
{
    NodeName = "Patrol Random";
    bNotifyTick = true;

    PatrolLocationKey.SelectedKeyName = FName("PatrolLocation");
    HomeLocationKey.SelectedKeyName = FName("HomeLocation");
}

EBTNodeResult::Type UBTTask_PatrolRandom::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("[PatrolRandom] No AIController"));
        return EBTNodeResult::Failed;
    }

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) 
    {
        UE_LOG(LogTemp, Error, TEXT("[PatrolRandom] No Enemy Pawn"));
        return EBTNodeResult::Failed;
    }
    
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("[PatrolRandom] No Blackboard"));
        return EBTNodeResult::Failed;
    }

    if (!Enemy->IsMovable())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Patrol] Enemy is not movable"));
        return EBTNodeResult::Failed;
    }

    // 순찰 속도 설정
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetPatrolSpeed();
    }


    // HomeLocation 가져오기
    FVector HomeLocation = BB->GetValueAsVector(HomeLocationKey.SelectedKeyName);
    
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(AIController->GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[Patrol] No NavSystem"));
        return EBTNodeResult::Failed;
    }

    if (HomeLocation.IsZero())
    {
        HomeLocation = Enemy->GetActorLocation();
        BB->SetValueAsVector(HomeLocationKey.SelectedKeyName, HomeLocation);
    }
    UE_LOG(LogTemp, Warning, TEXT("[Patrol] HomeLocation: %s"), *HomeLocation.ToString());

    FNavLocation ProjectedLoc;
    bool bIsOnNavMesh = NavSystem->ProjectPointToNavigation(
        HomeLocation,
        ProjectedLoc,
        FVector(500, 500, 500)  // 검색 범위
    );

    if (!bIsOnNavMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("[Patrol] HomeLocation is not on NavMesh!"));
        // HomeLocation을 NavMesh 위로 이동
        HomeLocation = ProjectedLoc.Location;
    }

    // 랜덤 위치 찾기
    float PatrolRadius = Enemy->GetPatrolRadius();

    UE_LOG(LogTemp, Log, TEXT("[Patrol] Searching (Home: %s, Radius: %.1f)"),
        *HomeLocation.ToString(), PatrolRadius);

    FNavLocation RandomLocation;
    ANavigationData* NavData = NavSystem->GetDefaultNavDataInstance();
    if (!NavData)
    {
        UE_LOG(LogTemp, Error, TEXT("[Patrol] No NavMesh found in world!"));
        return EBTNodeResult::Failed;
    }
    FSharedConstNavQueryFilter QueryFilter = UNavigationQueryFilter::GetQueryFilter(
        *NavData,
        AIController,
        nullptr
    );

    bool bFound = NavSystem->GetRandomReachablePointInRadius(
        HomeLocation,
        PatrolRadius,
        RandomLocation,
        nullptr,  // NavData
        QueryFilter
    );

    if (!bFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BT] Patrol Random: Could not find random location"));
        // 디버깅: NavMesh 존재 확인
        if (NavData)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Patrol] NavMesh exists but no valid point found. Try increasing PatrolRadius."));
        }

        return EBTNodeResult::Failed;
    }
    
    // 이동 시작
    UE_LOG(LogTemp, Log, TEXT("[BT] Patrol to: %s"), *RandomLocation.Location.ToString());

    BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(RandomLocation.Location);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(true);

    FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);

    if (MoveResult.Code == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }
    UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
    if (PathComp)
    {
        PathComp->OnRequestFinished.RemoveAll(this);

        PathComp->OnRequestFinished.AddWeakLambda(this,
            [this, &OwnerComp, AIController, WaitMin = WaitTimeMin, WaitMax = WaitTimeMax]
            (FAIRequestID RequestID, const FPathFollowingResult& Result)
            {
                if (Result.IsSuccess())
                {
                    // 도착 → 대기 시작
                    float WaitTime = FMath::RandRange(WaitMin, WaitMax);

                    UE_LOG(LogTemp, Log, TEXT("[Patrol] Arrived, waiting %.1fs"), WaitTime);

                    // Timer로 대기
                    FTimerHandle WaitTimerHandle;
                    AIController->GetWorld()->GetTimerManager().SetTimer(
                        WaitTimerHandle,
                        [this, &OwnerComp]()
                        {
                            UE_LOG(LogTemp, Log, TEXT("[Patrol] Wait complete"));
                            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                        },
                        WaitTime,
                        false
                    );
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Patrol] Move failed"));
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                }
            }
        );
    }

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PatrolRandom::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        AIController->StopMovement();
    }

    return EBTNodeResult::Aborted;
}

//void UBTTask_PatrolRandom::TickTask(
//    UBehaviorTreeComponent& OwnerComp,
//    uint8* NodeMemory,
//    float DeltaSeconds)
//{
//    AAIController* AIController = OwnerComp.GetAIOwner();
//    if (!AIController)
//    {
//        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        return;
//    }
//
//    
//}