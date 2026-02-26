#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_ChaseTarget.generated.h"

class AAIController;

/**
 * Target Actor를 추적하는 Task
 * FAIMoveRequest로 자동 경로 갱신
 */
UCLASS()
class TEAM3_PROJECT_API UBTTask_ChaseTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ChaseTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // 도착 판정 반지름 
    UPROPERTY(EditAnywhere, Category = "Chase", meta = (ClampMin = "0.0"))
    float AcceptanceRadius = 100.f;

    // 부분 경로 허용
    UPROPERTY(EditAnywhere, Category = "Chase")
    bool bAllowPartialPath = true;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

private:
    // 이동 완료 델리게이트 핸들러 
    UFUNCTION()
    void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

    // 캐시된 참조들
    UPROPERTY()
    UBehaviorTreeComponent* CachedOwnerComp = nullptr;

    TWeakObjectPtr<AAIController> CachedAIController;
    FAIRequestID CachedRequestID;
};