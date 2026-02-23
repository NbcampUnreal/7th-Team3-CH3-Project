#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WaitIdle.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_WaitIdle : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_WaitIdle();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Idle")
    float WaitTimeMin = 2.f;

    UPROPERTY(EditAnywhere, Category = "Idle")
    float WaitTimeMax = 5.f;

    UPROPERTY(EditAnywhere, Category = "Idle")
    bool bRandomizeWaitTime = true;

private:
    float WaitStartTime = 0.f;
    float WaitDuration = 0.f;
};
