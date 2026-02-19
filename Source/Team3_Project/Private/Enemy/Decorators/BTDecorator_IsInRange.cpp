#include "Enemy/Decorators/BTDecorator_IsInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UBTDecorator_IsInRange::UBTDecorator_IsInRange()
{
    NodeName = "Is In Range";

    TargetActorKey.SelectedKeyName = FName("TargetActor");

    // Observer 설정
    bNotifyBecomeRelevant = true;
    bNotifyTick = true;
}

bool UBTDecorator_IsInRange::CalculateRawConditionValue(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) return false;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor) return false;

    // ========================================
    // 거리 계산
    // ========================================
    float Distance = FVector::Dist(
        ControlledPawn->GetActorLocation(),
        TargetActor->GetActorLocation()
    );

    // ========================================
    // 범위 체크: MinRange < Distance <= MaxRange
    // ========================================
    bool bInRange = (Distance > MinRange) && (Distance <= MaxRange);

    return bInRange;
}