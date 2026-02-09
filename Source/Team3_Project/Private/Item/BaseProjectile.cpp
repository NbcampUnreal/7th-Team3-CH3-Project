// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


ABaseProjectile::ABaseProjectile()
{
 	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	RootComponent = CollisionComponent;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	InitialLifeSpan = 3.0f;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 수류탄 전용 3초 타이머
	if (!bExplodeOnHit)
	{
		FTimerHandle FuseTimerHandle;
		GetWorldTimerManager().SetTimer(
			FuseTimerHandle,
			this,
			&ABaseProjectile::Explode,
			3.0f,
			false
		);
	}
	
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 유효한 충돌 대상인지 확인
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		if (bExplodeOnHit)
		{
			Explode();
		}
	}
}

void ABaseProjectile::Explode()
{
	// 이펙트 재생
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (ExplosionRadius > 0.0f)
	{
		// 반경 내의 모든 액터에게 데미지 적용
		UGameplayStatics::ApplyRadialDamage(
			this,
			Damage,
			GetActorLocation(),
			ExplosionRadius,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			GetInstigatorController()
		);
	}
	else
	{
		UGameplayStatics::ApplyRadialDamage(
			this,
			Damage,
			GetActorLocation(),
			1.0f,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			GetInstigatorController()
		);
	}
	Destroy();
}

