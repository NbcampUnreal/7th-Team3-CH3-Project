#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

class AEnemyController;

UCLASS()
class TEAM3_PROJECT_API UStateBase : public UObject
{
	GENERATED_BODY()
public:
	void Init(AEnemyController* InController) { OwnerController = InController; }
	virtual void EnterState() {}
	virtual void TickState() {}
	virtual void ExitState() {}

protected:
	UPROPERTY()
	AEnemyController* OwnerController = nullptr;
};
