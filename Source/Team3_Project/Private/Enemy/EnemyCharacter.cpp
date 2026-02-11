#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	if (StatComp && TypeData)
	{
		float Health = TypeData->BaseHP;
		float Attack = TypeData->BaseAttack;
		float Defense = TypeData->BaseDefense;
		StatComp->InitializeStat(FName("Healh"), Health, 0.f, Health);
		StatComp->InitializeStat(FName("Attack"), Attack, 0.f, 200.f);
		StatComp->InitializeStat(FName("Defence"), Defense, 0.f, 200.f);
	}

	LeftAttackCoolTime = 0.f;
	bIsAttacking = false;
	bIsMovable = true;
	bIsForWave = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
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
		PlayAnimMontage(GetAttackMontage());
		
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
		return true;
	}
	return false;
}

void AEnemyCharacter::OnFinishAttack()
{
	bIsAttacking = false;
}


void AEnemyCharacter::OnHitted()
{
	StopAnimMontage();
	PlayAnimMontage(GetHittedMontage());
	bIsMovable = false;
}

void AEnemyCharacter::OnFinishHitted()
{
	bIsMovable = true;
}

void AEnemyCharacter::OnDead()
{
	StopAnimMontage();
	PlayAnimMontage(GetDeadMontage());
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
	// Todo 데미지 처리

	if (AEnemyController* EnemyController = Cast<AEnemyController>(GetController()))
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
