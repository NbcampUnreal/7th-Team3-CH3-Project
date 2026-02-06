#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));

	CurrentState = EEnemyState::Idle;
	DetectionRadius = 1000.f;
	ChaseRadius = 1500.f;
	AttackRadius = 200.f;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Todo Timer로 DetectPlayer 호출
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(DetectedTarget))
	{
		ChasePlayer();
	}
}

void AEnemyCharacter::DetectPlayer()
{
	// Todo SphereTrace로 주변 탐색
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
	// Todo 공격 몽타주 재생
}

void AEnemyCharacter::SpecialAttack()
{
	// Todo 특수 공격 몽타주 재생
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

	return StatComp->GetCuurrentStatValue(TEXT("Health"));
}

float AEnemyCharacter::GetMaxHealth() const
{
	return 0.0f;
}

float AEnemyCharacter::GetAttack() const
{
	return 0.0f;
}

float AEnemyCharacter::GetDefence() const
{
	return 0.0f;
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
