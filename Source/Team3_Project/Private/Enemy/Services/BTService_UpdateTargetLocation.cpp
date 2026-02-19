#include "Enemy/Services/BTService_UpdateTargetLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateTargetLocation::UBTService_UpdateTargetLocation()
{
    NodeName = "Update Target Location";

    // 0.5초마다 실행
    Interval = 0.5f;
    RandomDeviation = 0.1f;

    TargetActorKey.SelectedKeyName = FName("TargetActor");
    TargetLocationKey.SelectedKeyName = FName("TargetLocation");
}

void UBTService_UpdateTargetLocation::TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    // ========================================
    // TargetActor 가져오기
    // ========================================
    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        // 타겟 없음 → TargetLocation 클리어
        BB->ClearValue(TargetLocationKey.SelectedKeyName);
        return;
    }

    // ========================================
    // TargetLocation 갱신
    // ========================================
    FVector TargetLocation = TargetActor->GetActorLocation();
    BB->SetValueAsVector(TargetLocationKey.SelectedKeyName, TargetLocation);
}