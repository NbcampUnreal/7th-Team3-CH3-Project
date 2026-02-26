#include "Enemy/Tasks/BTTask_ChaseTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
    NodeName = "Chase Target";
    bNotifyTick = false;

    TargetActorKey.SelectedKeyName = FName("TargetActor");
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    // ========================================
    // 캐시 저장
    // ========================================
    CachedOwnerComp = &OwnerComp;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("[Chase] No AIController"));
        return EBTNodeResult::Failed;
    }
    CachedAIController = AIController;

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

    // ========================================
    // 추적 속도 설정
    // ========================================
    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Enemy->GetChaseSpeed();
    }

    // ========================================
    // 델리게이트 바인딩
    // ========================================
    AIController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);

    // ========================================
    // FAIMoveRequest로 이동 시작
    // ========================================
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAllowPartialPath(bAllowPartialPath);

    FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
    CachedRequestID = MoveResult.MoveId;

    // ========================================
    // ⭐ 수정: RequestID 로그 (uint32를 직접 출력)
    // ========================================
    UE_LOG(LogTemp, Log, TEXT("[Chase] Started chasing %s (RequestID: %u)"),
        *TargetActor->GetName(),
        static_cast<uint32>(CachedRequestID));  // ⭐ 명시적 캐스팅

    switch (MoveResult.Code)
    {
    case EPathFollowingRequestResult::RequestSuccessful:
        return EBTNodeResult::InProgress;

    case EPathFollowingRequestResult::AlreadyAtGoal:
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);
        UE_LOG(LogTemp, Log, TEXT("[Chase] Already at goal"));
        return EBTNodeResult::Succeeded;

    case EPathFollowingRequestResult::Failed:
    default:
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);
        UE_LOG(LogTemp, Error, TEXT("[Chase] MoveTo failed"));
        return EBTNodeResult::Failed;
    }
}

EBTNodeResult::Type UBTTask_ChaseTarget::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    if (AAIController* AIController = CachedAIController.Get())
    {
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);
        AIController->StopMovement();

        UE_LOG(LogTemp, Warning, TEXT("[Chase] Task aborted"));
    }

    return EBTNodeResult::Aborted;
}

void UBTTask_ChaseTarget::HandleMoveCompleted(
    FAIRequestID RequestID,
    EPathFollowingResult::Type Result)
{
    // ========================================
    // ⭐ 수정: Result 로그 (int32로 캐스팅)
    // ========================================
    UE_LOG(LogTemp, Log, TEXT("[Chase] MoveCompleted - RequestID: %u, Result: %d"),
        static_cast<uint32>(RequestID),  // ⭐ 명시적 캐스팅
        static_cast<int32>(Result));     // ⭐ 명시적 캐스팅

    // 델리게이트 해제
    if (AAIController* AIController = CachedAIController.Get())
    {
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ChaseTarget::HandleMoveCompleted);
    }

    if (!CachedOwnerComp)
    {
        UE_LOG(LogTemp, Error, TEXT("[Chase] CachedOwnerComp is null!"));
        return;
    }

    // ========================================
    // Task 완료 처리
    // ========================================
    const bool bSuccess = (Result == EPathFollowingResult::Success);

    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[Chase] Reached target successfully"));
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Chase] Failed to reach target"));
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
    }
}