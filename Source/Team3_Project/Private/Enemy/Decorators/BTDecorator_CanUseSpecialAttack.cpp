#include "Enemy/Decorators/BTDecorator_CanUseSpecialAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/SpecialAttackComponent.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "Player/PlayerCharacter.h"

UBTDecorator_CanUseSpecialAttack::UBTDecorator_CanUseSpecialAttack()
{
    NodeName = "Can Use Special Attack";

    LastSpecialAttackTimeKey.SelectedKeyName = FName("LastSpecialAttackTime");
    ConsecutiveAttacksKey.SelectedKeyName = FName("ConsecutiveAttacks");
    TargetActorKey.SelectedKeyName = FName("TargetActor");

    AttackID = FName("None");
    bUseChanceSystem = true;

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

    // 1. AttackID 유효성 체크
    if (AttackID == FName("None"))
    {
        UE_LOG(LogTemp, Error, TEXT("[Decorator] CanUseSpecialAttack: AttackID not set!"));
        return false;
    }

    // 2. SpecialAttackComponent 가져오기
    const USpecialAttackComponent* const SpecialAttackComponent = Enemy->GetSpecialAttackComponent();
    if (!SpecialAttackComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[Decorator] No SpecialAttackComponent"));
        return false;
    }

    // 3. AttackID로 공격 찾기
    USpecialAttackBase* Attack = SpecialAttackComponent->GetAttackByID(AttackID);
    if (!Attack)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Decorator] Attack '%s' not found"), *AttackID.ToString());
        return false;
    }

    // 4. Target 가져오기
    APlayerCharacter* TargetActor = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || TargetActor->IsDead()) return false;

    // 5. CanExecute 체크 (거리, 남은 시간 등)
    if (!Attack->CanExecute(Enemy, TargetActor))
    {
        return false;
    }

    // 7. 확률 체크 (옵션)
    if (bUseChanceSystem)
    {
        int32 AttackCount = BB->GetValueAsInt(ConsecutiveAttacksKey.SelectedKeyName);

        float Chance = BaseChance + (AttackCount * ChancePerAttack);
        Chance = FMath::Clamp(Chance, 0.f, MaxChance);

        float Random = FMath::FRand();  // 0.0 ~ 1.0

        bool bSuccess = Random <= Chance;

        UE_LOG(LogTemp, Log, TEXT("[Decorator] '%s' - Count: %d, Chance: %.2f%%, Random: %.2f, Result: %s"),
            *AttackID.ToString(), AttackCount, Chance * 100.f, Random, bSuccess ? TEXT("SUCCESS") : TEXT("FAIL"));

        return bSuccess;
    }

    // 확률 시스템 사용 안 함 → 항상 성공
    UE_LOG(LogTemp, Log, TEXT("[Decorator] '%s' - Always pass (no chance system)"), *AttackID.ToString());
    return true;
}
