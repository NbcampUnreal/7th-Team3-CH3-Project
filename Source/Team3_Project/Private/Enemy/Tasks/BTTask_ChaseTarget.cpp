#include "Enemy/Tasks/BTTask_ChaseTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
    NodeName = "Chase Target";
    bNotifyTick = false;
    bNotifyTaskFinished = true;

    TargetActorKey.SelectedKeyName = FName("TargetActor");
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    CachedOwnerComp = &OwnerComp;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    CachedAICon = AIController;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BT] Chase: No target"));
        return EBTNodeResult::Failed;
    }

    AIController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);
    AIController->SetFocus(TargetActor);
    UE_LOG(LogTemp, Warning, TEXT("Focus=%s"), *GetNameSafe(AIController->GetFocusActor()));
    // ========================================
    // 추적 속도 설정
    // ========================================
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetChaseSpeed();
    }

    // ========================================
    // 이동 시작
    // ========================================
    UE_LOG(LogTemp, Log, TEXT("[BT] Start Chasing: %s"), *TargetActor->GetName());

    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToActor(
        TargetActor,
        AcceptanceRadius,
        bStopOnOverlap,
        true,  // bUsePathfinding
        false,  // bCanStrafe
        nullptr,
        bAllowPartialPath
    );

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] Chase: MoveToActor failed"));
        return EBTNodeResult::Failed;
    }

    LastMoveTime = AIController->GetWorld()->GetTimeSeconds();
    return EBTNodeResult::InProgress;
}

//void UBTTask_ChaseTarget::TickTask(
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
//    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
//    if (!BB)
//    {
//        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        return;
//    }
//
//    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
//    if (!TargetActor)
//    {
//        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        return;
//    }
//
//    // ========================================
//    // 주기적으로 경로 갱신
//    // ========================================
//    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
//    if (CurrentTime - LastMoveTime >= PathUpdateInterval)
//    {
//        AIController->MoveToActor(
//            TargetActor,
//            AcceptanceRadius,
//            bStopOnOverlap,
//            true,
//            true,
//            nullptr,
//            bAllowPartialPath
//        );
//
//        LastMoveTime = CurrentTime;
//    }
//
//    // ========================================
//    // 이동 상태 체크
//    // ========================================
//    UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
//    if (PathComp)
//    {
//        // 경로 찾기 실패했는지 체크
//        if (!PathComp->GetPath().IsValid() || !PathComp->GetPath()->IsValid())
//        {
//            UE_LOG(LogTemp, Warning, TEXT("[BT] Chase: No valid path"));
//            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        }
//    }
//}

void UBTTask_ChaseTarget::OnTaskFinished(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    EBTNodeResult::Type TaskResult)
{
    if (TaskResult == EBTNodeResult::Failed || TaskResult == EBTNodeResult::Aborted)
    {
        if (AAIController* AIController = OwnerComp.GetAIOwner())
        {
            AIController->StopMovement();
        }
    }
}

void UBTTask_ChaseTarget::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if (AAIController* AIController = CachedAICon.Get())
    {
        // 델리게이트 해제
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);

        AIController->ClearFocus(EAIFocusPriority::Gameplay);
    }
    if (!CachedOwnerComp) return;

    const bool bSuccess = Result == EPathFollowingResult::Type::Success;
    FinishLatentTask(*CachedOwnerComp, bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
}