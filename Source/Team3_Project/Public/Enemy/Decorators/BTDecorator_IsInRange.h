#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInRange.generated.h"

/**
 * 거리 범위 체크 Decorator
 * - MinRange ~ MaxRange 사이에 있는지 확인
 * - TargetActor와의 거리 체크
 */
UCLASS()
class TEAM3_PROJECT_API UBTDecorator_IsInRange : public UBTDecorator
{
	GENERATED_BODY()
public:
    UBTDecorator_IsInRange();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, Category = "Range")
    float MinRange = 0.f;

    UPROPERTY(EditAnywhere, Category = "Range")
    float MaxRange = 300.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
