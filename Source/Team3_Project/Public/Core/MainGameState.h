// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStart, FString, WaveMessage, float, MaxWaveTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveEnd, int32, BonusPoint, bool, bIsWaveClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveFire);

class AEnemySpawner;
class AEventZone;
class AEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStart, FString, Message, float, WaveTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveEnd, bool, bSuccess, int32, Bonus);

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

	//현재웨이브에서 획득한 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaveSystem")
	int32 CurrentScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMonsterClass")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(VisibleAnywhere, Category = "SpanwedActors")
	TMap<int32, TWeakObjectPtr<AEnemySpawner>> EnemySpawners;
	UPROPERTY(VisibleAnywhere, Category = "SpanwedActors")
	TMap<int32, TWeakObjectPtr<AEventZone>> EventZones;
	
	FTimerDelegate EnemySpawnDelegate;
	FTimerHandle WaveStartTimer;
	FTimerHandle WaveEndTimer;

	//웨이브 시작 이벤트
	UPROPERTY(BlueprintAssignable, Category = "WaveSystem")
	FOnWaveStart OnWaveStart;
	//웨이브 종료 이벤트
	UPROPERTY(BlueprintAssignable, Category = "WaveSystem")
	FOnWaveEnd OnWaveEnd;

public:
	UPROPERTY(BlueprintAssignable)
	FOnWaveStart OnWaveStart;

	UPROPERTY(BlueprintAssignable)
	FOnWaveEnd OnWaveEnd;

	UPROPERTY(BlueprintAssignable)
	FOnWaveFire OnWaveFire;

public:

	UFUNCTION()
	void SpawnMonster(const int32 Id);

	virtual void BeginPlay() override;

	void FindSpawner(const int32 Id ,class AEnemySpawner* Spawner);
	void FindEventZone(const int32 Id, class AEventZone* EventZone);
	void OnTriggerEvent(const int32 Id, bool bIsPlayerIn);
	void WaveStart();
	void WaveEnd();

	void OnGameStart();
	void OnGameOver();
	void OnOpenMenu();

private:
	bool bIsRunningSpawner = false;
	bool bPlayerOnBox;
};
