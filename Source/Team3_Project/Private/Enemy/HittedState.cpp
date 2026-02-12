#include "Enemy/HittedState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyController.h"
#include "Animation/AnimInstance.h"

void UHittedState::EnterState()
{
    if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
    {
        Character->OnHitted();
    }
}

void UHittedState::TickState()
{
}

void UHittedState::ExitState()
{
    if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
    {
        Character->OnFinishHitted();
    }
}
