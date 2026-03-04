#include "Enemy/BossEnemyCharacter.h"
#include "Shared/Component/SpecialAttackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABossEnemyCharacter::ABossEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABossEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // ========================================
    // Phase 0에서 시작
    // ========================================
    CurrentPhase = 0;

    if (SpecialAttackComp)
    {
        SpecialAttackComp->SwitchPhase(CurrentPhase);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Boss] %s initialized at Phase %d"),
        *GetName().ToString(), CurrentPhase);
}

void ABossEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ========================================
    // Phase 전환 체크 (HP 기반)
    // ========================================
    if (!bIsTransitioningPhase && !IsDead())
    {
        CheckPhaseTransition();
    }
}

void ABossEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(PhaseTransitionTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ABossEnemyCharacter::CheckPhaseTransition()
{
    if (!StatComp) return;

    float HealthRatio = GetCurrentHealth() / GetMaxHealth();

    // ========================================
    // 현재 Phase보다 낮은 HP 임계값에 도달했는지 확인
    // ========================================
    for (int32 i = CurrentPhase; i < PhaseHealthThresholds.Num(); ++i)
    {
        if (HealthRatio <= PhaseHealthThresholds[i])
        {
            SwitchToPhase(i + 1);
            break;
        }
    }
}

void ABossEnemyCharacter::SwitchToPhase(int32 NewPhase)
{
    if (NewPhase == CurrentPhase)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Boss] Already in Phase %d"), NewPhase);
        return;
    }

    if (bIsTransitioningPhase)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Boss] Already transitioning"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Boss] ===== PHASE TRANSITION: %d → %d ====="),
        CurrentPhase, NewPhase);

    // ========================================
    // Phase 전환 시작
    // ========================================
    bIsTransitioningPhase = true;
    CurrentPhase = NewPhase;

    // 이동 정지
    DeactiveMove();
    StopAnimMontage();

    // SpecialAttackComponent에 Phase 전환 알림
    if (SpecialAttackComp)
    {
        SpecialAttackComp->SwitchPhase(CurrentPhase);
    }

    // ========================================
    // VFX & SFX
    // ========================================
    if (PhaseTransitionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            PhaseTransitionEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    if (PhaseTransitionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            PhaseTransitionSound,
            GetActorLocation()
        );
    }

    // ========================================
    // 무적 타이머 (2초)
    // ========================================
    GetWorldTimerManager().SetTimer(
        PhaseTransitionTimerHandle,
        this,
        &ABossEnemyCharacter::FinishPhaseTransition,
        PhaseTransitionInvincibilityTime,
        false
    );

    // ========================================
    // Delegate 브로드캐스트
    // ========================================
    OnPhaseChangedSignature.Broadcast(CurrentPhase);
}

void ABossEnemyCharacter::FinishPhaseTransition()
{
    bIsTransitioningPhase = false;
    ActiveMove();

    UE_LOG(LogTemp, Warning, TEXT("[Boss] Phase %d transition complete"), CurrentPhase);
}

float ABossEnemyCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    // ========================================
    // Phase 전환 중 무적
    // ========================================
    if (bIsTransitioningPhase)
    {
        UE_LOG(LogTemp, Log, TEXT("[Boss] Invincible during phase transition"));
        return 0.f;
    }

    // ========================================
    // 부모 클래스 데미지 처리
    // ========================================
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}