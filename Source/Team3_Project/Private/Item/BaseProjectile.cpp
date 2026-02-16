// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"

#define ECC_Weapon ECC_GameTraceChannel1

ABaseProjectile::ABaseProjectile()
{
 	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->BodyInstance.SetInstanceNotifyRBCollision(true);

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

	ExplosionEffect = nullptr;
	ExplosionRadius = 0.0f;
	Damage = 0.0f;
	bExplodeOnHit = true;

	InitialLifeSpan = 0.0f;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}

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
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
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
		TArray<FOverlapResult> OverlapResults;
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(ExplosionRadius);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

		bool bOverlapped = GetWorld()->OverlapMultiByObjectType(
			OverlapResults,
			GetActorLocation(),
			FQuat::Identity,
			ObjectQueryParams,
			CollisionShape
		);

		if (bOverlapped)
		{
			TArray<AActor*> DamagedActors;

			for (const FOverlapResult& Result : OverlapResults)
			{
				AActor* OverlappedActor = Result.GetActor();
				UPrimitiveComponent* OverlappedComponent = Result.GetComponent();

				if (!OverlappedActor || OverlappedActor == this || OverlappedActor == GetOwner() || DamagedActors.Contains(OverlappedActor))
				{
					continue;
				}

				FVector LaunchDirection = (OverlappedActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				LaunchDirection += FVector(0.0f, 0.0f, 0.5f); // 수직 방향으로도 약간의 임펄스 추가
				LaunchDirection.Normalize();

				float Distance = FVector::Dist(GetActorLocation(), OverlappedActor->GetActorLocation());
				float ForceAlpha = FMath::Clamp(1.0f - (Distance / ExplosionRadius), 0.0f, 1.0f);
				float ImpulseStrength = ForceAlpha * Damage * 10.0f; // 임펄스 강도는 피해량에 비례하도록 조정

				DamagedActors.Add(OverlappedActor);

				ACharacter* HitCharacter = Cast<ACharacter>(OverlappedActor);

				if (HitCharacter)
				{
					UGameplayStatics::ApplyDamage(
						OverlappedActor,
						Damage,
						GetInstigatorController(),
						this,
						UDamageType::StaticClass()
					);

					USkeletalMeshComponent* MeshComp = HitCharacter->GetMesh();
					if (MeshComp && MeshComp->IsSimulatingPhysics())
					{
						MeshComp->AddImpulse(
							LaunchDirection * ImpulseStrength * 7.0f,
							NAME_None,
							true
						);
					}
				}
				else if (OverlappedComponent && OverlappedComponent->IsSimulatingPhysics())
				{
					OverlappedComponent->AddImpulse(
						LaunchDirection * ImpulseStrength,
						NAME_None,
						true
					);

					FVector RandomTorque = FMath::VRand() * FMath::RandRange(10.0f, 100.0f) * ForceAlpha;

					OverlappedComponent->AddAngularImpulseInRadians(
						RandomTorque,
						NAME_None,
						true
					);

					UGameplayStatics::ApplyDamage(
						OverlappedActor,
						Damage,
						GetInstigatorController(),
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			ExplosionRadius,
			16,
			FColor::Red,
			false,
			2.0f
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

