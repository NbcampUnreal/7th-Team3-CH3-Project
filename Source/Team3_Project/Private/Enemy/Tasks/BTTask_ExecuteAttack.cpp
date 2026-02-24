#include "Enemy/Tasks/BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/Character.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
    NodeName = "Execute Attack";
    bNotifyTick = true;

    LastAttackTimeKey.SelectedKeyName = FName("LastAttackTime");
    AttackCountKey.SelectedKeyName = FName("AttackCount");
    ConsecutiveAttacksKey.SelectedKeyName = FName("ConsecutiveAttacks");
    TargetActorKey.SelectedKeyName = FName("TargetActor");
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    // 타겟 바라보기
    if (AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)))
    {
        AIController->SetFocus(Target);
    }

    // 공격 실행
    if (Enemy->Attack())
    {
        UE_LOG(LogTemp, Log, TEXT("[BT] Execute Attack"));

        // 쿨타임 기록
        float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
        BB->SetValueAsFloat(LastAttackTimeKey.SelectedKeyName, CurrentTime);

        // 공격 횟수 증가
        int32 ConsecutiveCount = BB->GetValueAsInt(ConsecutiveAttacksKey.SelectedKeyName);
        BB->SetValueAsInt(ConsecutiveAttacksKey.SelectedKeyName, ConsecutiveCount + 1);

        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_ExecuteAttack::TickTask(
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

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    // 공격 완료 체크 (IsAttackable == true면 공격 끝남)
    if (Enemy->IsAttackable())  // 쿨타임 돌아옴 = 공격 완료
    {
        // Focus 해제
        AIController->ClearFocus(EAIFocusPriority::Gameplay);
        UE_LOG(LogTemp, Log, TEXT("[BT] Attack Finished"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}