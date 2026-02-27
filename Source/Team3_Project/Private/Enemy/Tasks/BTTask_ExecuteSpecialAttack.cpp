#include "Enemy/Tasks/BTTask_ExecuteSpecialAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"

UBTTask_ExecuteSpecialAttack::UBTTask_ExecuteSpecialAttack()
{
    NodeName = "Execute Special Attack";
    bNotifyTick = true;

    LastSpecialAttackTimeKey.SelectedKeyName = FName("LastSpecialAttackTime");
    ConsecutiveAttacksKey.SelectedKeyName = FName("ConsecutiveAttacks");
    TargetActorKey.SelectedKeyName = FName("TargetActor");
    AttackID = FName("None");
}

EBTNodeResult::Type UBTTask_ExecuteSpecialAttack::ExecuteTask(
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
    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (TargetActor)
    {
        FVector Direction = TargetActor->GetActorLocation() - Enemy->GetActorLocation();
        Direction.Z = 0.f;

        if (!Direction.IsNearlyZero())
        {
            FRotator NewRotation = Direction.Rotation();
            Enemy->SetActorRotation(NewRotation);
        }
    }

    // 특수 공격 실행
    if (AttackID == FName("None"))
    {
        UE_LOG(LogTemp, Error, TEXT("[BT] SpecialAttack: AttackID not set!"));
        return EBTNodeResult::Failed;
    }

    bool bSuccess = Enemy->ExecuteSpecialAttackByID(AttackID, TargetActor);

    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BT] SpecialAttack '%s' failed"), *AttackID.ToString());
        return EBTNodeResult::Failed;
    }

    // 쿨타임 기록
    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
    BB->SetValueAsFloat(LastSpecialAttackTimeKey.SelectedKeyName, CurrentTime);

    // 공격 횟수 리셋
    BB->SetValueAsInt(ConsecutiveAttacksKey.SelectedKeyName, 0);

    UE_LOG(LogTemp, Log, TEXT("[BT] SpecialAttack '%s' started"), *AttackID.ToString());

    return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteSpecialAttack::TickTask(
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

    // ========================================
    // 공격 애니메이션 끝났는지 체크
    // ========================================
    if (Enemy->IsAttackable())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BT] Special Attack Finished"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}