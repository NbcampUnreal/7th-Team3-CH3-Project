#pragma once

#include "CoreMinimal.h"
#include "Enemy/StateBase.h"
#include "PatrolState.generated.h"

UCLASS()
class TEAM3_PROJECT_API UPatrolState : public UStateBase
{
	GENERATED_BODY()

public:
	virtual void EnterState() override;
	virtual void TickState() override;
	virtual void ExitState() override;
};
