#include "Shared/Component/SpecialAttackComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/SpecialAttackData.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "GameFramework/CharacterMovementComponent.h"

USpecialAttackComponent::USpecialAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USpecialAttackComponent::BeginPlay()
{
    Super::BeginPlay();

    // ========================================
    // Owner Enemy 캐싱
    // ========================================
    OwnerEnemy = Cast<AEnemyCharacter>(GetOwner());
    if (!OwnerEnemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpecialAttackComponent] Owner is not AEnemyCharacter!"));
        return;
    }

    // ========================================
    // Phase 0 시작 (일반 Enemy는 1개만)
    // ========================================
    if (PhaseDataAssets.Num() > 0)
    {
        SetSpecialAttackData(PhaseDataAssets[0]);
    }
}

void USpecialAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ========================================
    // 쿨타임 감소
    // ========================================
    for (auto& Pair : AttackCooldowns)
    {
        if (Pair.Value > 0.f)
        {
            Pair.Value -= DeltaTime;
            if (Pair.Value < 0.f)
            {
                Pair.Value = 0.f;
            }
        }
    }
}

void USpecialAttackComponent::SetSpecialAttackData(USpecialAttackData* NewData)
{
    if (!NewData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SpecialAttackComponent] SetSpecialAttackData: NewData is null"));
        return;
    }

    CurrentSpecialAttackData = NewData;

    // ========================================
    // 쿨타임 초기화 (새로운 DataAsset)
    // ========================================
    AttackCooldowns.Empty();

    UE_LOG(LogTemp, Log, TEXT("[SpecialAttackComponent] Loaded SpecialAttackData with %d attacks"),
        CurrentSpecialAttackData->SpecialAttacks.Num());
}

void USpecialAttackComponent::SwitchPhase(int32 PhaseIndex)
{
    if (!PhaseDataAssets.IsValidIndex(PhaseIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("[SpecialAttackComponent] Invalid PhaseIndex: %d"), PhaseIndex);
        return;
    }

    CurrentPhaseIndex = PhaseIndex;
    SetSpecialAttackData(PhaseDataAssets[PhaseIndex]);

    UE_LOG(LogTemp, Warning, TEXT("[SpecialAttackComponent] ===== Switched to Phase %d ====="), PhaseIndex);
}

bool USpecialAttackComponent::ExecuteSpecialAttackByID(FName AttackID, AActor* TargetActor)
{
    if (!OwnerEnemy || !CurrentSpecialAttackData)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpecialAttackComponent] No OwnerEnemy or CurrentSpecialAttackData"));
        return false;
    }

    // ========================================
    // DataAsset에서 공격 찾기 (읽기 전용!)
    // ========================================
    USpecialAttackBase* Attack = CurrentSpecialAttackData->GetAttackByID(AttackID);
    if (!Attack)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SpecialAttackComponent] Attack '%s' not found"), *AttackID.ToString());
        return false;
    }

    // ========================================
    // 쿨타임 체크 (Component에서 관리)
    // ========================================
    float* LeftCooldown = AttackCooldowns.Find(AttackID);
    if (LeftCooldown && *LeftCooldown > 0.f)
    {
        UE_LOG(LogTemp, Log, TEXT("[SpecialAttackComponent] '%s' is on cooldown: %.1fs"),
            *AttackID.ToString(), *LeftCooldown);
        return false;
    }

    // ========================================
    // CanExecute 체크 (거리, 조건 등)
    // ========================================
    if (!Attack->CanExecute(OwnerEnemy, TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("[SpecialAttackComponent] Cannot execute '%s'"), *AttackID.ToString());
        return false;
    }

    // ========================================
    // 몽타주 재생
    // ========================================
    if (UAnimMontage* Montage = Attack->GetMontage())
    {
        OwnerEnemy->PlayAnimMontage(Montage);
    }

    // ========================================
    // 이동 제어 및 Motage를 위한 FlyMode 전환
    // ========================================
    if (!Attack->CanMoveWhileAttacking())
    {
        OwnerEnemy->DeactiveMove();

        if (UCharacterMovementComponent* MoveComp = OwnerEnemy->GetCharacterMovement())
        {
            MoveComp->SetMovementMode(MOVE_Flying);  // Root Motion 적용 위해 Flying 모드로 전환
        }
    }

    // ========================================
    // 쿨타임 시작 (Component에서 관리!)
    // ========================================
    AttackCooldowns.Add(AttackID, Attack->GetCooldown());

    // ========================================
    // 상태 저장
    // ========================================
    CurrentSpecialAttack = Attack;
    CurrentTargetActor = TargetActor;

    UE_LOG(LogTemp, Log, TEXT("[SpecialAttackComponent] Executed: %s (Cooldown: %.1fs)"),
        *AttackID.ToString(), Attack->GetCooldown());

    return true;
}

void USpecialAttackComponent::TriggerSpecialAttackEffect()
{
    if (!CurrentSpecialAttack || !OwnerEnemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpecialAttackComponent] No CurrentSpecialAttack!"));
        return;
    }

    // ========================================
    // 실제 공격 실행 (DataAsset의 함수 호출)
    // ========================================
    CurrentSpecialAttack->Execute(OwnerEnemy, CurrentTargetActor);

    UE_LOG(LogTemp, Log, TEXT("[SpecialAttackComponent] Effect triggered: %s"),
        *CurrentSpecialAttack->GetAttackID().ToString());
}

void USpecialAttackComponent::OnFinishSpecialAttack()
{
    if (!OwnerEnemy)
        return;

    // ========================================
    // 이동 복구
    // ========================================
    if (CurrentSpecialAttack && !CurrentSpecialAttack->CanMoveWhileAttacking())
    {
        OwnerEnemy->ActiveMove();

        // Movement 모드 복구
        if (UCharacterMovementComponent* MoveComp = OwnerEnemy->GetCharacterMovement())
        {
            MoveComp->SetMovementMode(MOVE_Walking);
        }
    }

    // ========================================
    // 상태 초기화
    // ========================================
    CurrentSpecialAttack = nullptr;
    CurrentTargetActor = nullptr;

    UE_LOG(LogTemp, Log, TEXT("[SpecialAttackComponent] Special Attack finished"));
}

USpecialAttackBase* USpecialAttackComponent::GetAttackByID(FName AttackID) const
{
    if (!CurrentSpecialAttackData)
        return nullptr;

    return CurrentSpecialAttackData->GetAttackByID(AttackID);
}

bool USpecialAttackComponent::IsAttackReady(FName AttackID) const
{
    const float* LeftCooldown = AttackCooldowns.Find(AttackID);
    return !LeftCooldown || *LeftCooldown <= 0.f;
}

float USpecialAttackComponent::GetCooldownLeft(FName AttackID) const
{
    const float* LeftCooldown = AttackCooldowns.Find(AttackID);
    return LeftCooldown ? *LeftCooldown : 0.f;
}

TArray<USpecialAttackBase*> USpecialAttackComponent::GetAvailableAttacks(AActor* Target) const
{
    TArray<USpecialAttackBase*> Available;

    if (!CurrentSpecialAttackData || !OwnerEnemy)
        return Available;

    return CurrentSpecialAttackData->GetAvailableAttacks(OwnerEnemy, Target);
}

USpecialAttackBase* USpecialAttackComponent::GetCurrentAttack() const
{
    return CurrentSpecialAttack;
}
