#include "Enemy/DeadState.h"
#include "Enemy/Controllers/EnemyController.h"
#include "Enemy/EnemyCharacter.h"
#include "Perception/AIPerceptionComponent.h"

void UDeadState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Dead"));

	if (!OwnerController) return;
	// 이동 정지
	OwnerController->StopMovement();
	// 상태 전이 락
	OwnerController->SetStateLocked(true);

	// 인지 중단
	if (UAIPerceptionComponent* Perception = OwnerController->FindComponentByClass<UAIPerceptionComponent>())
	{
		Perception->ForgetAll();
		Perception->SetActive(false);
	}

	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
	{
		EnemyCharacter->OnDead();
	}
}

void UDeadState::TickState()
{
}

void UDeadState::ExitState()
{
}