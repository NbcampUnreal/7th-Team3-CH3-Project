#pragma once

#include "CoreMinimal.h"
#include "Enemy/StateBase.h"
#include "IdleState.generated.h"

UCLASS()
class TEAM3_PROJECT_API UIdleState : public UStateBase
{
	GENERATED_BODY()
	
public:
	virtual void EnterState() override;
	virtual void TickState() override;
	virtual void ExitState() override;
};
