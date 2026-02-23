#include "Enemy/Tasks/BTTask_HandleDeath.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"

UBTTask_HandleDeath::UBTTask_HandleDeath()
{
    NodeName = "Handle Death";
    bNotifyTick = true;
    bIsDeadKey.SelectedKeyName = FName("bIsDead");
}

EBTNodeResult::Type UBTTask_HandleDeath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    // ========================================
    // 사망 처리 시작
    // ========================================
    UE_LOG(LogTemp, Warning, TEXT("[BT] Handle Death - %s"), *(Enemy->GetName().ToString()));

    // AI 중지
    AIController->StopMovement();
    AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));

    // 사망 애니메이션 또는 Ragdoll
    Enemy->OnDead();

    // Blackboard 플래그
    BB->SetValueAsBool(bIsDeadKey.SelectedKeyName, true);

    // 시간 기록
    StartTime = AIController->GetWorld()->GetTimeSeconds();

    return EBTNodeResult::InProgress;
}

void UBTTask_HandleDeath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - StartTime;

    if (Enemy->IsDead() && !Enemy->IsPlayingRootMotion())
    {
        // 몽타주 완료 → Ragdoll 활성화 (아직 안 했으면)
        if (!Enemy->IsRagdollEnabled())
        {
            Enemy->EnableRagdoll();
        }
    }

    if (ElapsedTime >= DestroyDelay)
    {
        UE_LOG(LogTemp, Log, TEXT("[BT] Destroying enemy: %s"), *(Enemy->GetName().ToString()));
        Enemy->Destroy();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
