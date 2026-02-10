#include "Enemy/ChaseState.h"
#include "Enemy/EnemyController.h"

void UChaseState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Chase"));
	OwnerController->MoveToPlayer();
}

void UChaseState::TickState()
{
	if (OwnerController->IsPlayerInRange(OwnerController->GetAttackRange()))
	{
		OwnerController->ChangeState(OwnerController->GetAttackState());
		return;
	}
	else if (OwnerController->IsPlayerInRange(OwnerController->GetChaseRange()) == false)
	{
		OwnerController->ChangeState(OwnerController->GetIdleState());
		return;
	}

	// Todo 이동 중인 지 체크 후 이동중이 아니라면 이동
	if (!(OwnerController->IsMoving()))
	{
		OwnerController->MoveToPlayer();
	}
}

void UChaseState::ExitState()
{
	OwnerController->StopMovement();
}
