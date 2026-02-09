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
	
	ChangeState(EEnemyState::Idle);
	// Todo Timer로 DetectPlayer 호출
	// GetWorld()->GetTimerManager().SetTimer(DetectionTimerHandle, this, &AEnemyCharacter::DetectPlayer, 0.1f, true);
}

//void AEnemyCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// 이동 or 공격 (임시)
//	//if (IsValid(DetectedTarget) || GetDistanceTo(DetectedTarget) < AttackRadius)
//	//{
//	//	Attack();
//	//}
//	//else if (IsValid(DetectedTarget) || GetDistanceTo(DetectedTarget) < ChaseRadius )
//	//{
//	//	ChasePlayer();
//	//}
//	//else if (IsValid(DetectedTarget))
//	//{
//	//	// Lost
//	//	DetectedTarget = nullptr;
//	//}
//}

//void AEnemyCharacter::DetectPlayer()
//{
//	if (IsValid(DetectedTarget)) return;
//
//	// Todo SphereTrace로 주변 탐색
//	FVector Start = GetActorLocation();
//	FVector Forward = GetActorForwardVector();
//
//	TArray<FHitResult> HitResults;
//	FCollisionQueryParams Params;
//	Params.AddIgnoredActor(this);
//
//	bool bHit = GetWorld()->SweepMultiByObjectType(
//		HitResults,
//		Start,
//		Start,
//		FQuat::Identity,
//		FCollisionObjectQueryParams(ECC_Pawn),
//		FCollisionShape::MakeSphere(DetectionRadius),
//		Params
//	);
//
//	DrawDebugSphere(GetWorld(), Start, DetectionRadius, 12, FColor::Red, false, 1.0f);
//
//	if (bHit)
//	{
//		for (const FHitResult& Hit : HitResults)
//		{
//			AActor* HitActor = Hit.GetActor();
//			if (!HitActor) continue;
//			
//			// PlayerCharacter로 Cast 후 DetectedTarget에 저장하기
//		}
//	}
//}

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
