#pragma once

#include "CoreMinimal.h"
#include "Enemy/StateBase.h"
#include "DeadState.generated.h"

UCLASS()
class TEAM3_PROJECT_API UDeadState : public UStateBase
{
	GENERATED_BODY()
public:
	virtual void EnterState() override;
	virtual void TickState() override;
	virtual void ExitState() override;

private:
	// DeadMontage가 끝났는지 체크하기 위한 플래그
	bool bDeathMontageFinished = false;

	// 몽타주 종료 감지용 (택1: 시간 기반 / 델리게이트 기반)
	float DeathMontageEndTime = 0.f;

	// 삭제 딜레이
	float DestroyDelay = 3.f;

	FTimerHandle DestroyTimerHandle;

private:
	void StartDestroyTimer(AActor* OwnerActor);
	bool IsDeathMontageFinished(const class AEnemyCharacter* EnemyCharacter) const;
};
