#include "Enemy/Tasks/BTTask_FollowSpline.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_FollowSpline::UBTTask_FollowSpline()
{
    NodeName = "Follow Spline";
    bNotifyTick = true;

    SplineIndexKey.SelectedKeyName = FName("SplineIndex");
    CurrentState = ESplineState::MovingToPoint;
}

USplineComponent* UBTTask_FollowSpline::GetSplineComponent(AAIController* Controller) const
{
    if (!Controller) return nullptr;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Controller->GetPawn());
    if (!Enemy) return nullptr;

    // Enemy에 붙어있는 SplineComponent 찾기
    return Enemy->FindComponentByClass<USplineComponent>();
}

EBTNodeResult::Type UBTTask_FollowSpline::ExecuteTask(
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
    // SplineComponent 가져오기
    // ========================================
    SplineComponent = GetSplineComponent(AIController);
    if (!SplineComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Follow Spline: No SplineComponent found"));
        return EBTNodeResult::Failed;
    }

    int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
    if (NumPoints < 2)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Follow Spline: Not enough spline points (%d)"), NumPoints);
        return EBTNodeResult::Failed;
    }

    // ========================================
    // 순찰 속도 설정
    // ========================================
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetPatrolSpeed();
    }

    // ========================================
    // 현재 인덱스 가져오기
    // ========================================
    CurrentIndex = BB->GetValueAsInt(SplineIndexKey.SelectedKeyName);

    // 유효성 체크
    if (CurrentIndex < 0 || CurrentIndex >= NumPoints)
    {
        CurrentIndex = 0;
        BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, CurrentIndex);
    }

    // ========================================
    // 다음 포인트로 이동
    // ========================================
    FVector TargetLocation = SplineComponent->GetLocationAtSplinePoint(
        CurrentIndex,
        ESplineCoordinateSpace::World
    );

    UE_LOG(LogTemp, Log, TEXT("[BT] Follow Spline: Moving to point %d/%d at %s"),
        CurrentIndex, NumPoints - 1, *TargetLocation.ToString());

    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
        TargetLocation,
        AcceptanceRadius,
        true,  // bStopOnOverlap
        true,  // bUsePathfinding
        false,
        true,
        nullptr,
        true   // bAllowPartialPath
    );

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Follow Spline: MoveToLocation failed"));
        return EBTNodeResult::Failed;
    }

    CurrentState = ESplineState::MovingToPoint;
    return EBTNodeResult::InProgress;
}

void UBTTask_FollowSpline::TickTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController || !SplineComponent)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    switch (CurrentState)
    {
        case ESplineState::MovingToPoint:
        {
            EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

            if (Status == EPathFollowingStatus::Idle)
            {
                // ✅ 도착했는지 확인
                UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
                if (PathComp && PathComp->DidMoveReachGoal())
                {
                    // 성공적으로 도착
                    UE_LOG(LogTemp, Log, TEXT("[BT] Spline: Arrived at point %d"), CurrentIndex);

                    AIController->StopMovement();

                    WaitStartTime = AIController->GetWorld()->GetTimeSeconds();
                    CurrentState = ESplineState::Waiting;
                }
                else
                {
                    // 실패
                    UE_LOG(LogTemp, Warning, TEXT("[BT] Spline: Failed to reach point %d"), CurrentIndex);
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                }
            }
        }
        break;

        case ESplineState::Waiting:
        {
            float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
            float ElapsedTime = CurrentTime - WaitStartTime;

            if (ElapsedTime >= WaitTimeAtPoint)
            {
                // 대기 완료 → 다음 포인트로
                int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
                CurrentIndex++;

                if (CurrentIndex >= NumPoints)
                {
                    if (bLoopPath)
                    {
                        CurrentIndex = 0;
                        UE_LOG(LogTemp, Log, TEXT("[BT] Spline: Looping back to start"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Log, TEXT("[BT] Spline: Reached end, no loop"));
                        BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, 0);
                        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                        return;
                    }
                }

                BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, CurrentIndex);

                UE_LOG(LogTemp, Log, TEXT("[BT] Spline: Moving to next point"));
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
        }
        break;
    }
}