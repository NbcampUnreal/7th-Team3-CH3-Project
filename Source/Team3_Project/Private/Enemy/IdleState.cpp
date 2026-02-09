#include "Enemy/IdleState.h"
#include "Enemy/EnemyController.h"

void UIdleState::EnterState()
{
    UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Idle"));
    OwnerController->StopMovement();
}

void UIdleState::TickState()
{
    if (OwnerController->IsPlayerInRange(OwnerController->GetSightRange()))
    {
        OwnerController->ChangeState(OwnerController->GetChaseState());
        return;
    }

    // Todo 자동 이동 or 동작 모드 전환
}

void UIdleState::ExitState()
{
}
