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
    bNotifyTick = false;

    SplineIndexKey.SelectedKeyName = FName("SplineIndex");
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
    // 이동 가능 체크
    // ========================================
    if (!Enemy->IsMovable())
    {
        return EBTNodeResult::Failed;
    }

    // ========================================
    // SplineComponent 가져오기
    // ========================================
    USplineComponent* Spline = Enemy->FindComponentByClass<USplineComponent>();
    if (!Spline)
    {
        UE_LOG(LogTemp, Error, TEXT("[FollowSpline] No SplineComponent"));
        return EBTNodeResult::Failed;
    }

    int32 NumPoints = Spline->GetNumberOfSplinePoints();
    if (NumPoints < 2)
    {
        UE_LOG(LogTemp, Error, TEXT("[FollowSpline] Not enough points: %d"), NumPoints);
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
    int32 CurrentIndex = BB->GetValueAsInt(SplineIndexKey.SelectedKeyName);

    if (CurrentIndex < 0 || CurrentIndex >= NumPoints)
    {
        CurrentIndex = 0;
        BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, CurrentIndex);
    }

    // ========================================
    // 다음 포인트로 이동
    // ========================================
    FVector TargetLocation = Spline->GetLocationAtSplinePoint(
        CurrentIndex,
        ESplineCoordinateSpace::World
    );

    UE_LOG(LogTemp, Log, TEXT("[FollowSpline] Moving to point %d/%d"), CurrentIndex, NumPoints - 1);

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(TargetLocation);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(true);

    FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);

    if (MoveResult.Code == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    // Lambda로 완료 처리
    UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
    if (PathComp)
    {
        PathComp->OnRequestFinished.RemoveAll(this);

        PathComp->OnRequestFinished.AddWeakLambda(this,
            [this, &OwnerComp, AIController, BB, NumPoints, CurrentIndex, WaitTime = WaitTimeAtPoint]
            (FAIRequestID RequestID, const FPathFollowingResult& Result)
            {
                if (Result.IsSuccess())
                {
                    UE_LOG(LogTemp, Log, TEXT("[FollowSpline] Arrived at point %d"), CurrentIndex);

                    // 다음 인덱스 계산
                    int32 NextIndex = CurrentIndex + 1;

                    if (NextIndex >= NumPoints)
                    {
                        if (bLoopPath)
                        {
                            NextIndex = 0;
                            UE_LOG(LogTemp, Log, TEXT("[FollowSpline] Looping back to start"));
                        }
                        else
                        {
                            UE_LOG(LogTemp, Log, TEXT("[FollowSpline] Reached end"));
                            BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, 0);
                            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                            return;
                        }
                    }

                    BB->SetValueAsInt(SplineIndexKey.SelectedKeyName, NextIndex);

                    UE_LOG(LogTemp, Log, TEXT("[FollowSpline] Arrived successfully"));
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                }
                else
                {
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                }
            }
        );
    }

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_FollowSpline::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        AIController->StopMovement();
    }

    return EBTNodeResult::Aborted;
}