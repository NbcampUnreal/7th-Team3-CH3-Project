#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	if (StatComp)
	{
		StatComp->InitializeStat(FName("Healh"), 100.f, 0.f, 100.f);
		StatComp->InitializeStat(FName("Attack"), 50.f, 0.f, 200.f);
		StatComp->InitializeStat(FName("Defence"), 50.f, 0.f, 200.f);
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyCharacter::Attack()
{
	// Todo 공격 몽타주 재생
}

void AEnemyCharacter::SpecialAttack()
{
	//// Todo 특수 공격 몽타주 재생
}

void AEnemyCharacter::OnFinishAttack()
{
}


void AEnemyCharacter::OnHitted()
{
}

void AEnemyCharacter::OnDead()
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