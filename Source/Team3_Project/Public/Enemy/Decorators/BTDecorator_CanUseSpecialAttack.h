#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanUseSpecialAttack.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTDecorator_CanUseSpecialAttack : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_CanUseSpecialAttack();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
    // 체크할 특수 공격 ID
    UPROPERTY(EditAnywhere, Category = "SpecialAttack")
    FName AttackID = FName("None");

    // 확률 시스템 사용 여부 (false면 항상 성공)
    UPROPERTY(EditAnywhere, Category = "SpecialAttack")
    bool bUseChanceSystem = true;

    // 확률 설정
    UPROPERTY(EditAnywhere, Category = "SpecialAttack", meta = (EditCondition = "AttackType == ESpecialAttackType::Melee"))
    float BaseChance = 0.1f;  // 10% 기본 확률(원거리에서의 특수 공격의 경우 1.0으로 설정)

    UPROPERTY(EditAnywhere, Category = "SpecialAttack", meta = (EditCondition = "AttackType == ESpecialAttackType::Melee"))
    float ChancePerAttack = 0.15f;  // 공격마다 +15% (원거리에서는 0으로 설정)

    UPROPERTY(EditAnywhere, Category = "SpecialAttack", meta = (EditCondition = "AttackType == ESpecialAttackType::Melee"))
    float MaxChance = 0.8f;  // 최대 80% (원거리에서는 1.0으로 설정)

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastSpecialAttackTimeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector ConsecutiveAttacksKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
