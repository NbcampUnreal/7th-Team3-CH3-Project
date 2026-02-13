#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	

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

	if (StatComp && TypeData)
	{
		float Health = TypeData->BaseHP;
		float Attack = TypeData->BaseAttack;
		float Defense = TypeData->BaseDefense;
		StatComp->InitializeStat(FName("Health"), Health, 0.f, Health);
		StatComp->InitializeStat(FName("Attack"), Attack, 0.f, 200.f);
		StatComp->InitializeStat(FName("Defence"), Defense, 0.f, 200.f);
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeftAttackCoolTime > 0.f)
	{
		LeftAttackCoolTime = LeftAttackCoolTime - DeltaTime;
		if (LeftAttackCoolTime < 0.f) LeftAttackCoolTime = 0.f;
	}
}

bool AEnemyCharacter::Attack()
{
	if (IsAttackable())
	{
		LeftAttackCoolTime = GetAttackCoolTime();
		DeactiveMove();
		PlayAnimMontage(GetAttackMontage());
		//임시 공격처리
		TryMeleeHit();
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
		// 임시 공격처리
		TryMeleeHit();
		return true;
	}
	return false;
}

void AEnemyCharacter::OnFinishAttack()
{
	bIsAttacking = false;
	ActiveMove();
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
		
		ACharacter* Character = Cast<ACharacter>(HitActor);
		if (!IsValid(HitActor)) continue;
		if (HitActor == this) continue;
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
	PlayAnimMontage(GetHittedMontage());
	DeactiveMove();
	UE_LOG(LogTemp, Warning, TEXT("%s Hitted!"), *(GetActorLabel()));
}

void AEnemyCharacter::OnFinishHitted()
{
	ActiveMove();
}

void AEnemyCharacter::OnDead()
{
	StopAnimMontage();
	DeactiveMove();

	if (UAnimMontage* Montage = GetDeadMontage())
	{
		PlayAnimMontage(Montage);
	}
	else
	{
		EnableRagdoll();
		SetLifeSpan(5.f);
	}
}

void AEnemyCharacter::OnFinishDead()
{
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
		if (!bRagdollEnabled) EnableRagdoll();
		return DamageAmount;
	}
	// 피격 상태로 전환
	if (AEnemyController* EnemyController = Cast<AEnemyController>(GetController()))
	{
		EnemyController->ChangeState(EnemyController->GetHittedState());
	}

	// 데미지 처리
	ApplyDamageToStat(DamageAmount);

	// Hitted or Dead 상태 전환
	AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
	if (!EnemyController) return DamageAmount;
	if (StatComp->GetBaseStatValue(FName("Health")) <= 0.f)
	{
		EnemyController->ChangeState(EnemyController->GetDeadState());
		bIsDead = true;
	}
	else
	{
		EnemyController->ChangeState(EnemyController->GetHittedState());
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

void AEnemyCharacter::ApplyWaveFlag(bool bInWave)
{
	bIsForWave = bInWave;

	if (AEnemyController* EnemyController = Cast<AEnemyController>(GetController()))
	{
		EnemyController->TryApplyWaveSetup();
	}
}

void AEnemyCharacter::ActiveMove()
{
	bIsMovable = true;
}

void AEnemyCharacter::DeactiveMove()
{
	bIsMovable = false;
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
	UE_LOG(LogTemp, Warning, TEXT("Take %f damage! CurrentHealth : %f"), DamageAmount, StatComp->GetCurrentStatValue(FName("Health")));
}

void AEnemyCharacter::EnableRagdoll()
{
	if (!GetMesh()) return;

	bRagdollEnabled = true;
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
}
