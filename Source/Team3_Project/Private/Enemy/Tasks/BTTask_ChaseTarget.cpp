#include "Enemy/Tasks/BTTask_ChaseTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
    NodeName = "Chase Target";
    bNotifyTick = true;


    TargetActorKey.SelectedKeyName = FName("TargetActor");
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("[Chase] No AIController"));
        return EBTNodeResult::Failed;
    }

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[Chase] No Enemy Pawn"));
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("[Chase] No Blackboard"));
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Chase] No Target Actor"));
        return EBTNodeResult::Failed;
    }

    // 추적 속도 설정
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetChaseSpeed();
    }
    // FAIMoveRequest로 이동 시작
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(bAllowPartialPath);

    FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
    if (MoveResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        return EBTNodeResult::Succeeded;
    }
    else if (MoveResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        // PathFollowingComponent를 가져와 델리게이트 바인딩
        UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
        if (PathComp)
        {
            // 중복 바인딩 방지 (이전 이동 요청 리스너 제거)
            PathComp->OnRequestFinished.RemoveAll(this);

            // 람다 캡처를 사용하여 OwnerComp를 전달
            // AddWeakLambda는 객체(this)가 유효할 때만 실행되므로 안전합니다.
            PathComp->OnRequestFinished.AddWeakLambda(this, [this, &OwnerComp](FAIRequestID RequestID, const FPathFollowingResult& Result)
                {
                    // 여기서 OwnerComp는 캡처된 레퍼런스로, 해당 AI의 BT 컴포넌트를 정확히 가리킵니다.
                    FinishLatentTask(OwnerComp, Result.IsSuccess() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
                });
        }
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_ChaseTarget::AbortTask(
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
