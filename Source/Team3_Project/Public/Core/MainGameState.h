// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API AMainGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMainGameState();

	static AMainGameState* Get(const UWorld* WorldObject);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (WaveData))
	float MaxWaveTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (WaveData))
	float CurrentWaveTime;
	
	void OnGameStart();
	void OnGameOver();
	void OnOpenMenu();
	void OnStartWave();
	void OnEndWave();

};
