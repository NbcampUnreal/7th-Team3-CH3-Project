#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_ChaseTarget.generated.h"

class AAIController;

UCLASS()
class TEAM3_PROJECT_API UBTTask_ChaseTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_ChaseTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    // virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    UFUNCTION()
    void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

protected:
    UPROPERTY(EditAnywhere, Category = "Chase")
    float AcceptanceRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Chase")
    bool bStopOnOverlap = true;

    UPROPERTY(EditAnywhere, Category = "Chase")
    bool bAllowPartialPath = true;

    UPROPERTY(EditAnywhere, Category = "Chase")
    float PathUpdateInterval = 0.2f;  // 경로 갱신 주기

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

private:
    float LastMoveTime = 0.f;

    UPROPERTY()
    UBehaviorTreeComponent* CachedOwnerComp = nullptr;
    TWeakObjectPtr<AAIController> CachedAICon;
};
