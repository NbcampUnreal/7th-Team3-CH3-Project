#include "Enemy/EnemyController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyController::AEnemyController()
    :BehaviorTree(nullptr)
{
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (IsValid(BehaviorTree))
    {
        RunBehaviorTree(BehaviorTree);
    }
}

void AEnemyController::UpdateAI()
{
    if (IsValid(BehaviorTree))
    {
        // Todo AI 업데이트 로직 작성
    }
}
