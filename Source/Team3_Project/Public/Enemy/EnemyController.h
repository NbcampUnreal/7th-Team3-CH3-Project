#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTree;

UCLASS()
class TEAM3_PROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();


	UFUNCTION(BlueprintCallable, Category = "AIController")
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "AIController")
	void UpdateAI();

protected:
	UBehaviorTree* BehaviorTree;
};
