#include "Enemy/Tasks/BTTask_WaitIdle.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_WaitIdle::UBTTask_WaitIdle()
{
    NodeName = "Wait Idle";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WaitIdle::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    // ========================================
    // 이동 완전 정지
    // ========================================
    AIController->StopMovement();

    if (UCharacterMovementComponent* MoveComp = Enemy->GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
    }

    // ========================================
    // 대기 시간 설정
    // ========================================
    if (bRandomizeWaitTime)
    {
        WaitDuration = FMath::RandRange(WaitTimeMin, WaitTimeMax);
    }
    else
    {
        WaitDuration = WaitTimeMin;
    }

    WaitStartTime = AIController->GetWorld()->GetTimeSeconds();

    UE_LOG(LogTemp, Log, TEXT("[BT] Wait Idle: %.1f seconds"), WaitDuration);

    return EBTNodeResult::InProgress;
}

void UBTTask_WaitIdle::TickTask(
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

    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - WaitStartTime;

    if (ElapsedTime >= WaitDuration)
    {
        UE_LOG(LogTemp, Log, TEXT("[BT] Wait Idle: Finished"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}