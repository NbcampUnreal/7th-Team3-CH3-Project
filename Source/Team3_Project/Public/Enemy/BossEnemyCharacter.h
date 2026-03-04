#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "BossEnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);

/**
 * Boss 전용 Enemy 클래스
 * - Phase 시스템
 * - HP 기반 Phase 전환
 * - Boss 전용 UI/이펙트
 */
UCLASS()
class TEAM3_PROJECT_API ABossEnemyCharacter : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    ABossEnemyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // ========================================
    // Phase 시스템
    // ========================================

    /** 현재 Phase (0-based) */
    UFUNCTION(BlueprintPure, Category = "Boss")
    int32 GetCurrentPhase() const { return CurrentPhase; }

    /** Phase 전환 (수동 또는 HP 기반) */
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void SwitchToPhase(int32 NewPhase);

    /** HP 비율로 Phase 자동 전환 체크 */
    void CheckPhaseTransition();

    // ========================================
    // Override
    // ========================================

    virtual float TakeDamage(
        float DamageAmount,
        FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    // ========================================
    // Delegates
    // ========================================

    UPROPERTY(BlueprintAssignable, Category = "Boss")
    FOnPhaseChanged OnPhaseChangedSignature;

protected:
    /** 현재 Phase */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
    int32 CurrentPhase = 0;

    /** Phase별 HP 임계값 (0.0 ~ 1.0) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phase")
    TArray<float> PhaseHealthThresholds = { 0.7f, 0.4f, 0.15f };

    /** Phase 전환 시 무적 시간 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phase")
    float PhaseTransitionInvincibilityTime = 2.f;

    /** Phase 전환 중인지 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
    bool bIsTransitioningPhase = false;

    /** Boss 전용 이펙트/사운드 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|VFX")
    UParticleSystem* PhaseTransitionEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Audio")
    USoundBase* PhaseTransitionSound;

private:
    /** Phase 전환 완료 타이머 */
    FTimerHandle PhaseTransitionTimerHandle;

    /** Phase 전환 완료 */
    void FinishPhaseTransition();
};