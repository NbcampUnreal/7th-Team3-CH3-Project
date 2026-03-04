#pragma once

#include "CoreMinimal.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "SpecialAttack_AOE.generated.h"

// ========================================
// Timer Data 구조체 (TimerHandle 포함!)
// ========================================
struct FAOETimerData
{
    float CurrentRadius = 0.f;
    float ElapsedTime = 0.f;
    TSet<TWeakObjectPtr<AActor>> DamagedActors;
    TWeakObjectPtr<AEnemyCharacter> OwnerPtr;
    FVector Center;
    float MaxRad;
    float Dur;
    float Growth;
    float DmgMult;
    FTimerHandle TimerHandle;
};

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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    TSubclassOf<class AAOEIndicator> IndicatorClass;
    // 최대 반지름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float MaxRadius = 500.f;

    // 지속 시간
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float Duration = 2.f;

    // 성장 속도
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float GrowthRate = 250.f;

    // 데미지 배율
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    float DamageMultiplier = 0.5f;

    // AOE 중심 오프셋 (Owner 기준)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
    FVector CenterOffset = FVector(0, 0, 0);
};