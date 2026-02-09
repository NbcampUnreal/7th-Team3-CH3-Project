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

	CurrentState = EEnemyState::Idle;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ChangeState(EEnemyState::Idle);
	// Todo Timer로 DetectPlayer 호출
	// GetWorld()->GetTimerManager().SetTimer(DetectionTimerHandle, this, &AEnemyCharacter::DetectPlayer, 0.1f, true);
}

void AEnemyCharacter::ChasePlayer()
{
	if (IsValid(DetectedTarget))
	{
		// Todo MoveToPlayer
	}
}

void AEnemyCharacter::Attack()
{
	if (CurrentState == EEnemyState::Attacking ||
		CurrentState == EEnemyState::Dead) return;
	// Todo 공격 몽타주 재생

	ChangeState(EEnemyState::Attacking);
}

void AEnemyCharacter::SpecialAttack()
{
	if (CurrentState == EEnemyState::Attacking ||
		CurrentState == EEnemyState::Dead) return;
	// Todo 특수 공격 몽타주 재생

	ChangeState(EEnemyState::Attacking);
}


void AEnemyCharacter::OnFinishAttack()
{
	// Todo 공격 종료 처리

	ChangeState(EEnemyState::Idle);
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

EEnemyState AEnemyCharacter::GetState() const
{
	return CurrentState;
}

ACharacter* AEnemyCharacter::GetDetectedTarget() const
{
	return IsValid(DetectedTarget) ? DetectedTarget.Get() : nullptr;
}

void AEnemyCharacter::ChangeState(EEnemyState NewState)
{
	if (CurrentState == NewState) return;

	EEnemyState OldState = CurrentState;
	CurrentState = NewState;

	OnEnemyStateChanged.Broadcast(CurrentState, NewState);
}
