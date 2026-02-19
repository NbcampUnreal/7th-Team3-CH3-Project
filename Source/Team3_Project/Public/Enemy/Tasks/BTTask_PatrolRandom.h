#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PatrolRandom.generated.h"

UCLASS()
class TEAM3_PROJECT_API UBTTask_PatrolRandom : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_PatrolRandom();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float AcceptanceRadius = 50.f;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float WaitTimeMin = 2.f;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float WaitTimeMax = 5.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector PatrolLocationKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector HomeLocationKey;

private:
    enum class EPatrolState : uint8
    {
        MovingToLocation,
        Waiting
    };

    EPatrolState CurrentState;
    float WaitStartTime = 0.f;
    float WaitDuration = 0.f;
};
