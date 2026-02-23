#include "Enemy/Tasks/BTTask_PlayHittedMontage.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Animation/AnimInstance.h"

UBTTask_PlayHittedMontage::UBTTask_PlayHittedMontage()
{
    NodeName = "Play Hitted Montage";
    bNotifyTick = true;

    bIsHittedKey.SelectedKeyName = FName("bIsHitted");
}

EBTNodeResult::Type UBTTask_PlayHittedMontage::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    UE_LOG(LogTemp, Log, TEXT("[BT] Play Hitted Montage"));

    // ========================================
    // 이동 정지 및 피격 처리
    // ========================================
    AIController->StopMovement();
    Enemy->OnHitted();  // 몽타주 재생

    // 몽타주 길이 계산
    if (UAnimMontage* HittedMontage = Enemy->GetHittedMontage())
    {
        MontageLength = HittedMontage->GetPlayLength();
        UE_LOG(LogTemp, Log, TEXT("[BT] Hitted montage length: %.2f"), MontageLength);
    }
    else
    {
        MontageLength = 0.5f;  // 기본값
        UE_LOG(LogTemp, Warning, TEXT("[BT] No hitted montage, using default duration: %.2f"), DefaultHittedDuration);
    }

    StartTime = AIController->GetWorld()->GetTimeSeconds();

    return EBTNodeResult::InProgress;
}

void UBTTask_PlayHittedMontage::TickTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    float CurrentTime = AIController->GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - StartTime;

    // ========================================
    // 몽타주 완료 체크
    // ========================================
    if (ElapsedTime >= MontageLength)
    {
        UE_LOG(LogTemp, Log, TEXT("[BT] Hitted Montage Finished"));

        // 피격 플래그 해제
        UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
        if (BB)
        {
            BB->SetValueAsBool(bIsHittedKey.SelectedKeyName, false);
        }

        Enemy->OnFinishHitted();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}