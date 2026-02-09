#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
UCLASS()
class TEAM3_PROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();


	UFUNCTION(BlueprintCallable, Category = "Controller")
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateAI();

protected:
	// Perception 이벤트 핸들러
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);

protected:
	// 감각
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	// 시각
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// 청각
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRadius;
};
