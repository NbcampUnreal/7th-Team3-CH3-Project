#include "Enemy/Decorators/BTDecorator_CanUseSpecialAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"

UBTDecorator_CanUseSpecialAttack::UBTDecorator_CanUseSpecialAttack()
{
    NodeName = "Can Use Special Attack";

    LastSpecialAttackTimeKey.SelectedKeyName = FName("LastSpecialAttackTime");
    ConsecutiveAttacksKey.SelectedKeyName = FName("ConsecutiveAttacks");

    bNotifyBecomeRelevant = true;
}

bool UBTDecorator_CanUseSpecialAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return false;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    // 1. 기본 공격 가능 체크
    if (!Enemy->IsAttackable())
    {
        return false;
    }

    // 2. 쿨타임 체크
    float LastTime = BB->GetValueAsFloat(LastSpecialAttackTimeKey.SelectedKeyName);
    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();

    if (CurrentTime - LastTime < SpecialAttackCooldown)
    {
        return false;  // 쿨타임 중
    }

    // 3. 확률 체크
    int32 AttackCount = BB->GetValueAsInt(ConsecutiveAttacksKey.SelectedKeyName);

    float Chance = BaseChance + (AttackCount * ChancePerAttack);
    Chance = FMath::Clamp(Chance, 0.f, MaxChance);

    float Random = FMath::FRand();  // 0.0 ~ 1.0

    bool bSuccess = Random <= Chance;

    UE_LOG(LogTemp, Log, TEXT("[SpecialAttack] Count: %d, Chance: %.2f%%, Random: %.2f, Result: %s"),
        AttackCount, Chance * 100.f, Random, bSuccess ? TEXT("SUCCESS") : TEXT("FAIL"));

    return bSuccess;
}
