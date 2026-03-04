#include "Enemy/SpecialAttack/SpecialAttack_Projectile.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

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

    // Spawn Location 계산
    FVector SpawnLocation;
    FRotator BaseRotation;

    if (Owner->GetMesh()->DoesSocketExist(SpawnSocket))
    {
        SpawnLocation = Owner->GetMesh()->GetSocketLocation(SpawnSocket);
        FVector ForwardOffset = Owner->GetActorForwardVector() * 50.f;  // 추가로 앞으로
        SpawnLocation += ForwardOffset;
    }
    else
    {
        // Socket 없으면 Capsule 밖으로 Spawn
        UCapsuleComponent* Capsule = Owner->GetCapsuleComponent();
        float CapsuleRadius = Capsule ? Capsule->GetScaledCapsuleRadius() : 50.f;

        FVector Forward = Owner->GetActorForwardVector();
        FVector Up = Owner->GetActorUpVector();

        // Capsule 반지름 + 추가 오프셋만큼 앞으로
        SpawnLocation = Owner->GetActorLocation()
            + Forward * (CapsuleRadius + SpawnForwardOffset)
            + Up * SpawnHeightOffset;
        // pawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
    }

    FVector Direction = (TargetActor->GetActorLocation() - SpawnLocation).GetSafeNormal();
    BaseRotation = Direction.Rotation();

    // 다중 투사체 발사
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    // 충돌 처리: 발사 시점에 Owner와 겹쳐도 무시
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

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

        // EnemyProjectile Spawn
        AEnemyProjectile* Projectile = Owner->GetWorld()->SpawnActor<AEnemyProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (Projectile)
        {
            // EnemyProjectile 초기화

            // Damage 설정 (BaseProjectile에 있는 프로퍼티)
            Projectile->Damage = Owner->GetAttack();

            // 유도 설정
            if (bIsHoming)
            {
                Projectile->SetHoming(bIsHoming, TargetActor, HomingAcceleration);
            }

            // 생존 시간
            Projectile->SetLifeSpan(ProjectileLifeSpan);

            if (UProjectileMovementComponent* ProjMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
            {
                ProjMovement->InitialSpeed = ProjectileSpeed;
                ProjMovement->MaxSpeed = ProjectileSpeed;

                UE_LOG(LogTemp, Warning, TEXT("[Projectile] Speed: %.1f, Velocity: %s, Active: %d"),
                    ProjMovement->InitialSpeed,
                    *ProjMovement->Velocity.ToString(),
                    ProjMovement->IsActive());
            }
            // ⭐ BeginPlay 이전에 Owner 무시 설정
            if (USphereComponent* CollisionComp = Projectile->FindComponentByClass<USphereComponent>())
            {
                CollisionComp->IgnoreActorWhenMoving(Owner, true);
                CollisionComp->MoveIgnoreActors.AddUnique(Owner);
            }

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