#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UStateBase;

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

	// FSM
	void ChangeState(UStateBase* NewState);

	// State Getter
	UStateBase* GetIdleState() const { return IdleState; }
	UStateBase* GetChaseState() const { return ChaseState; }
	UStateBase* GetAttackState() const { return AttackState; }

	bool IsPlayerInRange(float Range) const;
	void MoveToRandomLocation();
	void MoveToPlayer();
	bool IsMoving() const;

	float GetSightRange() const { return SightRange; }
	float GetChaseRange() const { return ChaseRange; }
	float GetAttackRange() const { return AttackRange; }
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

	// 감각 시각
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// 감각 청각
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<ACharacter> TargetActor;

private:
	UPROPERTY()
	UStateBase* CurrentState;

	UPROPERTY()
	UStateBase* IdleState;
	UPROPERTY()
	UStateBase* ChaseState;
	UPROPERTY()
	UStateBase* AttackState;

	float SightRange;
	float ChaseRange;
	float AttackRange;
};
