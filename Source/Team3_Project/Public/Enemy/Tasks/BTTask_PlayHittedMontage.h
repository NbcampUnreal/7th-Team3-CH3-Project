#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayHittedMontage.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_PlayHittedMontage : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_PlayHittedMontage();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector bIsHittedKey;
    UPROPERTY(EditAnywhere, Category = "Hitted")
    float DefaultHittedDuration = 0.5f;

private:
    float MontageLength = 0.f;
    float StartTime = 0.f;
};
