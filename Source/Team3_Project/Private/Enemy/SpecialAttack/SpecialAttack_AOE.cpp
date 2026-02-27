#include "Enemy/SpecialAttack/SpecialAttack_AOE.h"
#include "Enemy/EnemyCharacter.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Shared/Component/StatComponent.h"
#include "Engine/OverlapResult.h"  // ⭐ 추가!

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

    // ========================================
    // AOE 중심 위치
    // ========================================
    FVector AOECenter = Owner->GetActorLocation() + CenterOffset;

    UE_LOG(LogTemp, Log, TEXT("[AOE] Starting at %s, MaxRadius: %.1f, Duration: %.1f"),
        *AOECenter.ToString(), MaxRadius, Duration);

    // ========================================
    // ⭐ Timer Data 구조체 (TimerHandle 포함!)
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
        FTimerHandle TimerHandle;  // ⭐ 여기!
    };

    // ========================================
    // Shared Pointer로 관리
    // ========================================
    TSharedPtr<FAOETimerData> AOEData = MakeShared<FAOETimerData>();
    AOEData->OwnerPtr = Owner;
    AOEData->Center = AOECenter;
    AOEData->MaxRad = MaxRadius;
    AOEData->Dur = Duration;
    AOEData->Growth = GrowthRate;
    AOEData->DmgMult = DamageMultiplier;

    // ========================================
    // ⭐ SetTimer (AOEData->TimerHandle 사용)
    // ========================================
    World->GetTimerManager().SetTimer(
        AOEData->TimerHandle,  // ⭐ 구조체 멤버!
        [World, AOEData]()  // ⭐ AOEData만 캡처
        {
            // Owner 유효성 체크
            AEnemyCharacter* OwnerRef = AOEData->OwnerPtr.Get();
            if (!OwnerRef || !IsValid(OwnerRef))
            {
                UE_LOG(LogTemp, Warning, TEXT("[AOE] Owner destroyed, stopping AOE"));
                World->GetTimerManager().ClearTimer(AOEData->TimerHandle);  // ⭐ OK!
                return;
            }

            float DeltaTime = World->GetDeltaSeconds();
            AOEData->ElapsedTime += DeltaTime;

            // 반지름 증가
            AOEData->CurrentRadius = FMath::Min(
                AOEData->CurrentRadius + AOEData->Growth * DeltaTime,
                AOEData->MaxRad
            );

            // 범위 내 Actor 찾기
            TArray<FOverlapResult> OverlapResults;
            FCollisionShape SphereShape = FCollisionShape::MakeSphere(AOEData->CurrentRadius);
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(OwnerRef);

            bool bHit = World->OverlapMultiByChannel(
                OverlapResults,
                AOEData->Center,
                FQuat::Identity,
                ECC_Pawn,
                SphereShape,
                QueryParams
            );

            // 데미지 적용
            if (bHit)
            {
                for (const FOverlapResult& Result : OverlapResults)
                {
                    AActor* HitActor = Result.GetActor();
                    if (!HitActor || !IsValid(HitActor))
                        continue;

                    // 중복 데미지 체크
                    bool bAlreadyDamaged = false;
                    for (const TWeakObjectPtr<AActor>& DamagedActor : AOEData->DamagedActors)
                    {
                        if (DamagedActor.Get() == HitActor)
                        {
                            bAlreadyDamaged = true;
                            break;
                        }
                    }

                    if (bAlreadyDamaged)
                        continue;

                    // Player만 공격
                    if (APlayerCharacter* Player = Cast<APlayerCharacter>(HitActor))
                    {
                        // StatComponent로 Attack 가져오기
                        float BaseDamage = 10.f;
                        if (UStatComponent* StatComp = OwnerRef->FindComponentByClass<UStatComponent>())
                        {
                            BaseDamage = StatComp->GetCurrentStatValue(FName("Attack"));
                        }

                        float Damage = BaseDamage * AOEData->DmgMult;

                        UGameplayStatics::ApplyDamage(
                            HitActor,
                            Damage,
                            OwnerRef->GetController(),
                            OwnerRef,
                            UDamageType::StaticClass()
                        );

                        AOEData->DamagedActors.Add(HitActor);

                        UE_LOG(LogTemp, Log, TEXT("[AOE] Damaged %s (%.1f)"), *HitActor->GetName(), Damage);
                    }
                }
            }

#if WITH_EDITOR
            // Debug Draw
            DrawDebugSphere(
                World,
                AOEData->Center,
                AOEData->CurrentRadius,
                32,
                FColor::Red,
                false,
                0.1f,
                0,
                5.f
            );
#endif

            // 종료 조건
            if (AOEData->ElapsedTime >= AOEData->Dur)
            {
                UE_LOG(LogTemp, Log, TEXT("[AOE] Finished"));
                World->GetTimerManager().ClearTimer(AOEData->TimerHandle);  // ⭐ OK!
            }
        },
        0.05f,  // 20 FPS
        true    // 반복
    );
}