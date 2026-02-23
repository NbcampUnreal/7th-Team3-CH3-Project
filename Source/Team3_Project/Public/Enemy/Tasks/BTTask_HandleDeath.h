#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HandleDeath.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_HandleDeath : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_HandleDeath();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Death")
    float DestroyDelay = 5.f;  // Actor 제거까지 대기 시간

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector bIsDeadKey;

private:
    float StartTime = 0.f;
};
