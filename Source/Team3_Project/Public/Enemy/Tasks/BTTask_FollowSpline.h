#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowSpline.generated.h"

class USplineComponent;

UCLASS()
class TEAM3_PROJECT_API UBTTask_FollowSpline : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_FollowSpline();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Spline")
    float AcceptanceRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Spline")
    bool bLoopPath = true;

    UPROPERTY(EditAnywhere, Category = "Spline")
    float WaitTimeAtPoint = 2.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector SplineIndexKey;

private:
    enum class ESplineState : uint8
    {
        MovingToPoint,
        Waiting
    };

    ESplineState CurrentState;
    USplineComponent* SplineComponent = nullptr;
    int32 CurrentIndex = 0;
    float WaitStartTime = 0.f;

    USplineComponent* GetSplineComponent(AAIController* Controller) const;
};
