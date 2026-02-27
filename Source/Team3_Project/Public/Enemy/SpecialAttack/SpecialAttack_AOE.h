#pragma once

#include "CoreMinimal.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "SpecialAttack_AOE.generated.h"

/**
 * 원형 범위 공격
 */
UCLASS(Blueprintable)
class TEAM3_PROJECT_API USpecialAttack_AOE : public USpecialAttackBase
{
    GENERATED_BODY()

public:
    USpecialAttack_AOE();

    virtual void Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor) override;

protected:
    /** 최대 반지름 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float MaxRadius = 500.f;

    /** 지속 시간 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float Duration = 2.f;

    /** 성장 속도 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float GrowthRate = 250.f;

    /** 데미지 배율 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float DamageMultiplier = 0.5f;

    /** AOE 중심 오프셋 (Owner 기준) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    FVector CenterOffset = FVector(0, 0, 0);
};