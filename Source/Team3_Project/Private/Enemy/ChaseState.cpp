#include "Enemy/ChaseState.h"
#include "Enemy/Controllers/EnemyController.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UChaseState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Chase"));
	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = EnemyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = EnemyCharacter->GetChaseSpeed();
		}
	}
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
		if (!OwnerController->IsForWave())
		{
			OwnerController->ChangeState(OwnerController->GetIdleState());
			return;
		}
	}

	// Todo 이동 중인 지 체크 후 이동중이 아니라면 이동
	if (OwnerController->IsMovable() && !(OwnerController->IsMoving()))
	{
		OwnerController->MoveToPlayer();
	}
}

void UChaseState::ExitState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Exit Chase"));
	OwnerController->StopMovement();
}
