#include "Enemy/PatrolState.h"
#include "Enemy/EnemyController.h"

void UPatrolState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Patrol"));
}

void UPatrolState::TickState()
{
	if (OwnerController->IsPlayerInRange(OwnerController->GetChaseRange()))
	{
		OwnerController->ChangeState(OwnerController->GetChaseState());
		return;
	}

	if (OwnerController->IsMovable() && !OwnerController->IsMoving())
	{
		OwnerController->MoveToRandomLocation();
	}
}

void UPatrolState::ExitState()
{
}
