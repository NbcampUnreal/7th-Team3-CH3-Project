#include "Enemy/PatrolState.h"
#include "Enemy/EnemyController.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPatrolState::EnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Patrol"));
	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = EnemyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = EnemyCharacter->GetPatrolSpeed();
		}
	}

	if (OwnerController->IsMovable() && !OwnerController->IsMoving())
	{
		OwnerController->MoveToRandomLocation();
	}
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
	UE_LOG(LogTemp, Warning, TEXT("[AI] Exit Chase"));
}
