#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/Controllers/EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Player/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetForAIMove();

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));

	WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	LeftAttackCoolTime = 0.f;
	bIsAttacking = false;
	bIsMovable = true;
	bIsForWave = false;
	bIsDead = false;
	bRagdollEnabled = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (TypeData)
	{
		if (StatComp)
		{
			float Health = TypeData->BaseHP;
			float Attack = TypeData->BaseAttack;
			float Defense = TypeData->BaseDefense;
			float WhiteKarma = TypeData->WhiteKarma;
			float BlackKarma = TypeData->BlackKarma;
			StatComp->InitializeStat(FName("Health"), Health, 0.f, Health);
			StatComp->InitializeStat(FName("Attack"), Attack, 0.f, 200.f);
			StatComp->InitializeStat(FName("Defence"), Defense, 0.f, 200.f);
			StatComp->InitializeStat(FName("WhiteKarma"), WhiteKarma, 0.f, 200.f);
			StatComp->InitializeStat(FName("BlackKarma"), BlackKarma, 0.f, 200.f);
		}
		
		if (WeaponCollision)
		{
			FName SocketName = TypeData->SocketName;
			float WeaponRadius = TypeData->WeaponRadius;
			ensureMsgf(FMath::IsFinite(WeaponRadius), TEXT("WeaponRadius is not finite"));
			ensureMsgf(WeaponRadius >= 0.f, TEXT("WeaponRadius is negative: %f"), WeaponRadius);

			WeaponCollision->SetSphereRadius(WeaponRadius);
			WeaponCollision->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName
			);

			WeaponCollision->OnComponentBeginOverlap.AddDynamic(
				this,
				&AEnemyCharacter::OnWeaponBeginOverlap
			);
		}
	}

	ActiveMove();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeftAttackCoolTime > 0.f)
	{
		LeftAttackCoolTime = LeftAttackCoolTime - DeltaTime;
		if (LeftAttackCoolTime < 0.f) LeftAttackCoolTime = 0.f;
	}

#if WITH_EDITOR
	if (WeaponCollision && WeaponCollision->IsCollisionEnabled())
	{
		DrawDebugSphere(
			GetWorld(),
			WeaponCollision->GetComponentLocation(),
			WeaponCollision->GetScaledSphereRadius(),
			12,
			FColor::Red,
			false,
			-1.f,
			0,
			2.f
		);
	}
#endif
}

bool AEnemyCharacter::Attack()
{
	if (IsAttackable())
	{
		LeftAttackCoolTime = GetAttackCoolTime();
		PlayAnimMontage(GetAttackMontage());
		
		OnAttackSignature.Broadcast();
		return true;
	}

	return false;
}

bool AEnemyCharacter::SpecialAttack()
{
	if (IsAttackable())
	{
		LeftAttackCoolTime = GetAttackCoolTime();
		PlayAnimMontage(GetSpecialAttackMontage());
		DeactiveMove();

		OnSepcialAttackSignature.Broadcast();
		// 임시 공격처리
		//TryMeleeHit();
		return true;
	}
	return false;
}

void AEnemyCharacter::OnFinishAttack()
{
	bIsAttacking = false;
	OnFinishAttackSignature.Broadcast();
}

void AEnemyCharacter::OnFinishSpecialAttack()
{
	bIsAttacking = false;
	OnFinishSpecialAttackSignature.Broadcast();
}

void AEnemyCharacter::EnableWeaponCollision()
{
	if (bIsDead) return;
	if (WeaponCollision)
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ResetHitActors();  // 활성화와 동시에 Hit 목록 초기화

		UE_LOG(LogTemp, Log, TEXT("[Combat] Weapon collision enabled"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] WeaponCollision is null"));
	}
}

void AEnemyCharacter::DisableWeaponCollision()
{
	if (WeaponCollision)
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		UE_LOG(LogTemp, Log, TEXT("[Combat] Weapon collision disabled"));
	}
}

void AEnemyCharacter::ResetHitActors()
{
	HitActorsThisAttack.Empty();
	UE_LOG(LogTemp, Log, TEXT("[Combat] Hit actors reset"));
}


void AEnemyCharacter::TryMeleeHit()
{
	// 임시 공격 처리
	const float Radius = 100.f;
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MeleeSWeep), false);
	Params.AddIgnoredActor(this);
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * 300.f;
	const bool bHitAny = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		Shape,
		Params
	);

	// Debug
	{
		const float LifeTime = 1.0f;
		const bool bPersistent = false;

		const FColor Color = bHitAny ? FColor::Red : FColor::Green;

		// (1) 시작/끝 스피어
		DrawDebugSphere(GetWorld(), Start, Radius, 16, Color, bPersistent, LifeTime, 0, 2.0f);
		DrawDebugSphere(GetWorld(), End, Radius, 16, Color, bPersistent, LifeTime, 0, 2.0f);

		// (2) 스윕 경로: 캡슐(시작~끝을 잇는 형태)
		const FVector Center = (Start + End) * 0.5f;
		const FVector Dir = (End - Start);
		const float HalfHeight = Dir.Size() * 0.5f;

		// Dir가 0에 가까우면 회전 만들기 어려우니 가드
		const FQuat CapsuleRot = !Dir.IsNearlyZero()
			? FRotationMatrix::MakeFromZ(Dir.GetSafeNormal()).ToQuat()
			: FQuat::Identity;

		DrawDebugCapsule(
			GetWorld(),
			Center,
			HalfHeight,
			Radius,
			CapsuleRot,
			Color,
			bPersistent,
			LifeTime,
			0,
			2.0f
		);

		// (3) 실제 히트 지점들 표시(선택)
		for (const FHitResult& Hit : HitResults)
		{
			if (!Hit.bBlockingHit && !Hit.bStartPenetrating && !Hit.GetActor()) continue;

			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 12.f, FColor::Yellow, bPersistent, LifeTime);
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + Hit.ImpactNormal * 60.f,
				FColor::Cyan, bPersistent, LifeTime, 0, 1.0f);
		}
	}

	// 중복 타격 방지
	TSet<AActor*> DamagedActor;
	for (FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor)) continue;
		if (HitActor == this) continue;
		
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(HitActor);
		if (!IsValid(PlayerCharacter)) continue;
		// if (!HitActor->ActorHasTag(FName("Player"))) continue;
		if (DamagedActor.Contains(HitActor)) continue;

		DamagedActor.Add(HitActor);

		UGameplayStatics::ApplyDamage(
			HitActor,
			StatComp->GetCurrentStatValue(FName("Attack")),
			GetController(),	//EventInstigator
			this,		// DamageCauser
			UDamageType::StaticClass()
		);

		UE_LOG(LogTemp, Warning, TEXT("ApplyDamage To %s"), *(HitActor->GetActorLabel()));
	}
}

void AEnemyCharacter::OnHitted()
{
	StopAnimMontage();
	OnHittedSignature.Broadcast();

	if (UAnimMontage* Montage = GetHittedMontage())
	{
		PlayAnimMontage(Montage);
		DeactiveMove();
		UE_LOG(LogTemp, Warning, TEXT("%s Hitted!"), *(GetActorLabel()));
	}
	else
	{
		OnFinishHitted();
	}
}

void AEnemyCharacter::OnFinishHitted()
{
	ActiveMove();
	OnFinishHittedSignature.Broadcast();
}

void AEnemyCharacter::OnDead()
{
	bIsDead = true;
	StopAnimMontage();
	DeactiveMove();
	OnDeadSignature.Broadcast();

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	if (UAnimMontage* Montage = GetDeadMontage())
	{
		PlayAnimMontage(Montage);
	}
	else
	{
		OnFinishDead();
	}
}

void AEnemyCharacter::OnFinishDead()
{
	OnFinishDeadSignature.Broadcast();
	EnableRagdoll();
	DisableWeaponCollision();
	SetLifeSpan(5.f);
	UE_LOG(LogTemp, Warning, TEXT("Pause=%d Sim=%d"),
    GetMesh()->bPauseAnims,
    GetMesh()->IsSimulatingPhysics());
}

float AEnemyCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInsitagtor,
	AActor* DamageCauser
)
{
	if (bIsDead)
	{
		// if (!bRagdollEnabled) EnableRagdoll();
		return DamageAmount;
	}

	// 데미지 처리
	ApplyDamageToStat(DamageAmount);

	// Hitted or Dead 상태 전환
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return DamageAmount;

	if (StatComp->GetBaseStatValue(FName("Health")) <= 0.f)
	{
		OnDead();
	}
	else
	{
		OnHitted();
	}

	return DamageAmount;
}

FName AEnemyCharacter::GetName() const
{
	return EnemyName;
}
float AEnemyCharacter::GetCurrentHealth() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Health"));
}

float AEnemyCharacter::GetMaxHealth() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetMaxStatValue(TEXT("Health"));
}

float AEnemyCharacter::GetAttack() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Attack"));
}

float AEnemyCharacter::GetDefence() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Defence"));
}

float AEnemyCharacter::GetWhiteKarma() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("WhiteKarma"));
}

float AEnemyCharacter::GetBlackKarma() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("BlackKarma"));
}

bool AEnemyCharacter::IsAttackable()
{
	if (!FMath::IsNearlyZero(LeftAttackCoolTime)) return false;
	
	return !bIsAttacking;
}
bool AEnemyCharacter::IsMovable() const
{
	return bIsMovable;
}

bool AEnemyCharacter::IsForWave() const
{
	return bIsForWave;
}

bool AEnemyCharacter::IsDead() const
{
	return bIsDead;
}

bool AEnemyCharacter::IsRagdollEnabled() const
{
	return bRagdollEnabled;
}

void AEnemyCharacter::ApplyWaveFlag(bool bInWave)
{
	bIsForWave = bInWave;

	OnWaveModeSignature.Broadcast(bIsForWave);
}

void AEnemyCharacter::ActiveMove()
{
	bIsMovable = true;
	OnMovableChangedSignature.Broadcast(bIsMovable);
}

void AEnemyCharacter::DeactiveMove()
{
	bIsMovable = false;
	OnMovableChangedSignature.Broadcast(bIsMovable);
}

UAnimMontage* AEnemyCharacter::GetAttackMontage() const
{
	return TypeData ? TypeData->AttackMontage : nullptr;
}

UAnimMontage* AEnemyCharacter::GetSpecialAttackMontage() const
{
	return TypeData ? TypeData->SpecialAttackMontage : nullptr;
}

UAnimMontage* AEnemyCharacter::GetHittedMontage() const
{
	return TypeData ? TypeData->HittedMontage : nullptr;
}

UAnimMontage* AEnemyCharacter::GetDeadMontage() const
{
	return TypeData ? TypeData->DeadMontage : nullptr;
}

float AEnemyCharacter::GetAttackCoolTime() const
{
	return TypeData ? TypeData->AttackCoolTime : 5.f;
}

float AEnemyCharacter::GetPatrolSpeed() const
{
	return TypeData ? TypeData->PatrolMaxSpeed : 150.f;
}

float AEnemyCharacter::GetPatrolRadius() const
{
	return TypeData ? TypeData->PatrolRadius : 150.f;
}

float AEnemyCharacter::GetChaseSpeed() const
{
	return TypeData ? TypeData->ChaseMaxSpeed : 600.f;
}

void AEnemyCharacter::ApplyDamageToStat(float DamageAmount)
{
	float CurrentHealth = StatComp->GetBaseStatValue(FName("Health"));
	float Defence = StatComp->GetBaseStatValue(FName("Defence"));
	float Damage = FMath::Clamp(DamageAmount - Defence, 1, DamageAmount);

	StatComp->SetBaseStatValue(FName("Health"), CurrentHealth - Damage);
	UE_LOG(LogTemp, Warning, TEXT("Get %f damage! Defence : %f, Take ActualDamage : %f | CurrentHealth : %f"), DamageAmount, Defence, Damage, StatComp->GetCurrentStatValue(FName("Health")));
	
	OnHealthChangedSignature.Broadcast(
		StatComp->GetCurrentStatValue(FName("Health")),
		StatComp->GetMaxStatValue(FName("Health")));
}

void AEnemyCharacter::EnableRagdoll()
{
	if (!GetMesh()) return;

	bRagdollEnabled = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->bPauseAnims = true;
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
}

void AEnemyCharacter::SetForAIMove()
{
	bUseControllerRotationYaw = false;  // AI가 직접 회전하지 않으므로 false로 설정해야 함.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true; // 이동 방향으로 회전
	}
}

void AEnemyCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDead) return;
	if (!OtherActor || OtherActor == this) return;

	// 중복 타격 방지
	if (HitActorsThisAttack.Contains(OtherActor))
	{
		return; // 이미 맞춤
	}

	// Team3_ProjectCharacter만 공격 (필요시 수정)
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}

	// 데미지 적용
	float Damage = StatComp->GetCurrentStatValue(FName("Attack"));

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	// 타격 목록에 추가
	HitActorsThisAttack.Add(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("[Combat] Hit: %s (Damage: %.1f)"),
		*OtherActor->GetName(), Damage);
}
