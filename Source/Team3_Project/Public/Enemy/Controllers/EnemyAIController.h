#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class TEAM3_PROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
    AEnemyAIController();

    virtual void OnPossess(APawn* InPawn) override;
    
    // Wave 모드 설정
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetWaveMode(bool bEnabled);

protected:
    virtual void BeginPlay() override;

    // Perception 이벤트
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION()
    void OnTargetPerceptionForgotten(AActor* Actor);

    // Blackboard 초기화
    void InitializeBlackboard();

    UFUNCTION()
    void OnHitted();

    UFUNCTION()
    void OnFinishHitted();

    UFUNCTION()
    void OnDead();

protected:
    // Components & Config
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Hearing* HearingConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    // Range Settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Range")
    float SightRange = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Range")
    float ChaseRange = 1500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Range")
    float AttackRange = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Range")
    float SpecialAttackRange = 800.f;
};
