#include "Enemy/AttackState.h"
#include "Enemy/EnemyController.h"
#include "Enemy/EnemyCharacter.h"

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

	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
	{
		if (EnemyCharacter->Attack())
		{
			UE_LOG(LogTemp, Warning, TEXT("[AI] Attacking!"));
		}
		if (EnemyCharacter->SpecialAttack())
		{
			UE_LOG(LogTemp, Warning, TEXT("[AI] Special Attacking!"));
		}
	}
}

void UAttackState::ExitState()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] Exit Attack"));
	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
	{
		EnemyCharacter->OnFinishAttack();
	}
}
