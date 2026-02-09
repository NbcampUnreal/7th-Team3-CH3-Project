// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

UENUM(BlueprintType)
enum class EParkourType : uint8
{
	None,
	Vault,  // 낮은 벽 넘기
	Mantle, // 높은 벽 올라가기
	Climb   // 매달리기
};

// 분석 결과 담는 곳
USTRUCT(BlueprintType)
struct FParkourResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bCanParkour = false;

	UPROPERTY(BlueprintReadOnly)
	EParkourType Type = EParkourType::None;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector; // 도달해야 할 목표 지점
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM3_PROJECT_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UParkourComponent();

	// 캐릭터에서 호출할 메인 함수
	UFUNCTION(BlueprintCallable, Category = "Parkour")
	FParkourResult CheckObstacle(AActor* Owner);

protected:
	// 감지 거리 및 높이 설정 값들
	UPROPERTY(EditAnywhere, Category = "Parkour|Settings")
	float DetectionDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Parkour|Settings")
	float MinVaultHeight = 50.f;

	UPROPERTY(EditAnywhere, Category = "Parkour|Settings")
	float MaxVaultHeight = 120.f;

		
};
