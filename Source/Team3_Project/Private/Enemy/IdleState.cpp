#include "Enemy/IdleState.h"
#include "Enemy/EnemyController.h"

void UIdleState::EnterState()
{
    UE_LOG(LogTemp, Warning, TEXT("[AI] Enter Idle"));
    OwnerController->StopMovement();

    WaitTime = 0.f;
}

void UIdleState::TickState()
{
    if (OwnerController->IsPlayerInRange(OwnerController->GetSightRange()))
    {
        OwnerController->ChangeState(OwnerController->GetChaseState());
        return;
    }

    // Todo 자동 이동 or 동작 모드 전환
    WaitTime += GetWorld()->DeltaTimeSeconds;
    if (WaitTime > ChangeTime)
    {
        OwnerController->ChangeState(OwnerController->GetPatrolState());
    }
}

void UIdleState::ExitState()
{
    WaitTime = 0.f;
}
