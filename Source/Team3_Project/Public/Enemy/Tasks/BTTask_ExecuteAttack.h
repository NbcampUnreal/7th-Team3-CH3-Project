#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteAttack.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_ExecuteAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_ExecuteAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastAttackTimeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AttackCountKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector ConsecutiveAttacksKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
