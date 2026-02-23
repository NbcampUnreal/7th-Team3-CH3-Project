#include "Enemy/HittedState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/Controllers/EnemyController.h"
#include "Animation/AnimInstance.h"

void UHittedState::EnterState()
{
    UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Hitted"));
    if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
    {
        Character->OnHitted();
    }
}

void UHittedState::TickState()
{
    if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
    {
        if (!Character->GetHittedMontage())
        {
            OwnerController->ChangeState(OwnerController->GetIdleState());
        }
    }
}

void UHittedState::ExitState()
{
    UE_LOG(LogTemp, Warning, TEXT("[AI] Exit Hitted"));
    if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerController->GetPawn()))
    {
        Character->OnFinishHitted();
    }
}
