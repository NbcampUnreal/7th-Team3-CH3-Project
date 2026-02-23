#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteSpecialAttack.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_ExecuteSpecialAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_ExecuteSpecialAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastSpecialAttackTimeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector ConsecutiveAttacksKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
