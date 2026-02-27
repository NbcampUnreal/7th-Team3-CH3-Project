#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChaseTarget.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_ChaseTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ChaseTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Chase", meta = (ClampMin = "0.0"))
    float AcceptanceRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Chase")
    bool bAllowPartialPath = true;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};