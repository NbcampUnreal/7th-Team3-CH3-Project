// EnemyProjectile.cpp
#include "Enemy/EnemyProjectile.h"
#include "Enemy/EnemyCharacter.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemyProjectile::AEnemyProjectile()
{
}

void AEnemyProjectile::SetHoming(bool IsHoming, AActor* TargetActor, float InHomingAcceleration)
{
    bIsHoming = true;
    HomingTarget = TargetActor;
    HomingAcceleration = InHomingAcceleration;
}

void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Owner가 Enemy인지 확인
    AEnemyCharacter* OwnerEnemy = Cast<AEnemyCharacter>(GetOwner());
    if (!OwnerEnemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[EnemyProjectile] Owner is not AEnemyCharacter!"));
        Destroy();
        return;
    }

    // Instigator Controller 확인
    if (!GetInstigatorController())
    {
        UE_LOG(LogTemp, Warning, TEXT("[EnemyProjectile] No InstigatorController! Owner: %s"),
            *OwnerEnemy->GetName().ToString());
    }
}

void AEnemyProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 유도 미사일
    if (bIsHoming && HomingTarget && ProjectileMovementComponent)
    {
        if (!IsValid(HomingTarget))
        {
            bIsHoming = false;
            return;
        }

        FVector ToTarget = (HomingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector CurrentVelocity = ProjectileMovementComponent->Velocity;

        FVector NewVelocity = FMath::VInterpTo(
            CurrentVelocity,
            ToTarget * ProjectileMovementComponent->MaxSpeed,
            DeltaTime,
            HomingAcceleration / ProjectileMovementComponent->MaxSpeed
        );

        ProjectileMovementComponent->Velocity = NewVelocity;
        SetActorRotation(NewVelocity.Rotation());
    }
}

void AEnemyProjectile::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;  // 자기 자신이나 발사한 Enemy는 무시
    }

    // Player만 공격
    if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[EnemyProjectile] Hit Player: %s"), *OtherActor->GetName());

    // Instigator Controller 사용
    AController* InstigatorController = GetInstigatorController();
    if (!InstigatorController)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EnemyProjectile] No InstigatorController!"));
    }

    // 데미지 적용
    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        InstigatorController,  // Enemy의 Controller
        this,
        DamageType->StaticClass()
    );

    // 폭발
    if (bExplodeOnHit)
    {
        Explode();
    }
    else
    {
        Destroy();
    }
}