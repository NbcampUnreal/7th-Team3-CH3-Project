#include "Shared/Component/ParkourComponent.h"

UParkourComponent::UParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FParkourResult UParkourComponent::CheckObstacle(AActor* Owner)
{
	FParkourResult Result;
	if (!Owner) return Result;

	// 트레이스로 벽 감지하는 로직

	return Result;

}
