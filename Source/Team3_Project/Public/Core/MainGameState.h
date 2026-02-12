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
	
	//웨이브 전체시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveSystem")
	float MaxWaveTime;

	//웨이브 현재시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveSystem")
	float CurrentWaveTime;

	//최대 생성되는 몬스터 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveSystem")
	int32 MaxSpawnCount;

	//현재 생성되어있는 몬스터의 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveSystem")
	int32 CurrentSpawnCount;

	//현재웨이브에서 획득한 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveSystem")
	int32 CurrentScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	TArray<TObjectPtr<class AEnemySpawner>> EnemySpawners;

	FTimerHandle WaveStartTimer;
	FTimerHandle WaveEndTimer;

	virtual void BeginPlay() override;

	void FindSpawner();
	void SpawnMonster();
	void WaveStart();
	void WaveEnd();

	void OnGameStart();
	void OnGameOver();
	void OnOpenMenu();

};
