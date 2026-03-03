#include "Enemy/SpecialAttack/SpecialAttack_AOE.h"
#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Enemy/SpecialAttack/AOEIndicator.h"

USpecialAttack_AOE::USpecialAttack_AOE()
{
    AttackID = FName("AOE");
    MinRange = 0.f;
    MaxRange = 300.f;
    Cooldown = 8.f;
    bCanMoveWhileAttacking = false;
}

void USpecialAttack_AOE::Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor)
{
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("[AOE] No Owner"));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[AOE] No World"));
        return;
    }

    if (!IndicatorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[AOE] No IndicatorClass assigned"));
        return;
    }

    // AOE 중심 위치
    FVector AOECenter = Owner->GetActorLocation() + CenterOffset;

    UE_LOG(LogTemp, Log, TEXT("[AOE] Starting at %s, MaxRadius: %.1f, Duration: %.1f"),
        *AOECenter.ToString(), MaxRadius, Duration);

    // AOE Base Damage 가져오기
    // ========================================
    float BaseDamage = 10.f;
    if (UStatComponent* StatComp = Owner->FindComponentByClass<UStatComponent>())
    {
        BaseDamage = StatComp->GetCurrentStatValue(FName("Attack"));
    }

    // AOE Indicator 생성
    FTransform SpawnTransform;
    SpawnTransform.SetLocation(AOECenter);

    AAOEIndicator* Indicator = World->SpawnActor<AAOEIndicator>(
        IndicatorClass,
        SpawnTransform
    );

    if (Indicator)
    {
        // Indicator가 모든 것을 처리
        Indicator->StartAOE(
            Owner->GetController(),  // Instigator
            BaseDamage,              // 기본 데미지
            MaxRadius,               // 최대 반지름
            Duration,                // 지속 시간
            GrowthRate,              // 성장 속도
            DamageMultiplier         // 데미지 배율
        );
        UE_LOG(LogTemp, Log, TEXT("[AOE] Indicator spawned"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AOE] Failed to spawn Indicator"));
    }



}