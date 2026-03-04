#include "Enemy/Decorators/BTDecorator_CanAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Player/PlayerCharacter.h"

UBTDecorator_CanAttack::UBTDecorator_CanAttack()
{
    NodeName = "Can Attack";

    bNotifyBecomeRelevant = true;

    TargetActorKey.SelectedKeyName = FName("TargetActor");
}

bool UBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return false;

    APlayerCharacter* TargetActor = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor || TargetActor->IsDead()) return false;

    // IsAttackable() 체크 (쿨타임 + 공격 중 아님)
    return Enemy->IsAttackable(TargetActor);
}
