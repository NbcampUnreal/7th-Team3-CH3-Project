#include "Enemy/AttackState.h"
#include "Enemy/EnemyController.h"

void UAttackState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Attack"));
	OwnerController->StopMovement();
}

void UAttackState::TickState()
{
	if (OwnerController->IsPlayerInRange(OwnerController->GetAttackRange()) == false)
	{
		OwnerController->ChangeState(OwnerController->GetChaseState());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AI] Attacking!"));
}

void UAttackState::ExitState()
{
}
