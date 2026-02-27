#include "Enemy/SpecialAttack/SpecialAttack_Projectile.h"
#include "Enemy/EnemyCharacter.h"
#include "Item/BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

USpecialAttack_Projectile::USpecialAttack_Projectile()
{
    AttackID = FName("Projectile");
    MinRange = 300.f;
    MaxRange = 1500.f;
    Cooldown = 3.f;
}

void USpecialAttack_Projectile::Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor)
{
    if (!Owner || !TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("[Projectile] Invalid Owner or Target"));
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[Projectile] No ProjectileClass assigned"));
        return;
    }

    // ========================================
    // Spawn Location 계산
    // ========================================
    FVector SpawnLocation;
    FRotator BaseRotation;

    if (Owner->GetMesh()->DoesSocketExist(SpawnSocket))
    {
        SpawnLocation = Owner->GetMesh()->GetSocketLocation(SpawnSocket);
    }
    else
    {
        SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
    }

    FVector Direction = (TargetActor->GetActorLocation() - SpawnLocation).GetSafeNormal();
    BaseRotation = Direction.Rotation();

    // ========================================
    // 다중 투사체 발사
    // ========================================
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < ProjectileCount; ++i)
    {
        // 발사 각도 계산
        float AngleOffset = 0.f;

        if (ProjectileCount > 1)
        {
            float AngleStep = SpreadAngle / (ProjectileCount - 1);
            AngleOffset = -SpreadAngle / 2.f + (AngleStep * i);
        }

        FRotator SpawnRotation = BaseRotation;
        SpawnRotation.Yaw += AngleOffset;

        // ========================================
        // ⭐ BaseProjectile Spawn
        // ========================================
        ABaseProjectile* Projectile = Owner->GetWorld()->SpawnActor<ABaseProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (Projectile)
        {
            // ========================================
            // ⭐ BaseProjectile 초기화
            // ========================================

            // Damage 설정 (BaseProjectile에 있는 프로퍼티)
            Projectile->Damage = Owner->GetAttack();

            // ProjectileMovement 속도 설정
            if (UProjectileMovementComponent* ProjMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
            {
                ProjMovement->InitialSpeed = ProjectileSpeed;
                ProjMovement->MaxSpeed = ProjectileSpeed;
            }

            UE_LOG(LogTemp, Log, TEXT("[Projectile] Spawned #%d at angle %.1f, Damage: %.1f"),
                i, AngleOffset, Projectile->Damage);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Projectile] Failed to spawn projectile #%d"), i);
        }
    }
}