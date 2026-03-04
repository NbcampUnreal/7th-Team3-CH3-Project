// EnemyProjectile.cpp
#include "Enemy/EnemyProjectile.h"
#include "Enemy/EnemyCharacter.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
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

    if (ProjectileMovementComponent)
    {
        // UpdatedComponent 재설정
        if (!ProjectileMovementComponent->UpdatedComponent)
        {
            ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
            UE_LOG(LogTemp, Warning, TEXT("[EnemyProjectile] UpdatedComponent was null, reassigned"));
        }

        // 활성화 확인
        if (!ProjectileMovementComponent->IsActive())
        {
            ProjectileMovementComponent->Activate();
            UE_LOG(LogTemp, Warning, TEXT("[EnemyProjectile] ProjectileMovement was inactive, activated"));
        }

        // Velocity가 0인 경우 방향으로 재설정
        if (ProjectileMovementComponent->Velocity.IsNearlyZero())
        {
            FVector LaunchDirection = GetActorForwardVector();
            ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
            UE_LOG(LogTemp, Warning, TEXT("[EnemyProjectile] Velocity was zero, set to: %s"),
                *ProjectileMovementComponent->Velocity.ToString());
        }

        UE_LOG(LogTemp, Log, TEXT("[EnemyProjectile] BeginPlay - Speed: %.1f, Velocity: %s, Active: %d, UpdatedComp: %s"),
            ProjectileMovementComponent->InitialSpeed,
            *ProjectileMovementComponent->Velocity.ToString(),
            ProjectileMovementComponent->IsActive(),
            *GetNameSafe(ProjectileMovementComponent->UpdatedComponent));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[EnemyProjectile] No ProjectileMovementComponent!"));
    }
    CollisionComponent->OnComponentHit.Clear();  // 기존 바인딩 제거
    CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
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