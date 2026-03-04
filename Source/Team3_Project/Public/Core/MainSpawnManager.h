// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainSpawnManager.generated.h"

class AEventZone;
class AEnemySpawner;

UCLASS()
class TEAM3_PROJECT_API AMainSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	AMainSpawnManager();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnObjectClass | EventZone")
	TSubclassOf<AEventZone> EventZoneClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnObjectClass | EnemySpawner")
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnObjectClass | QuestItem")
	//TODO_@Core : 스폰할 아이템 클래스 등록

	void SpawnEventZone();
	void SpawnEnemySpawner();
	void SpawnQuestItem();
	void BindSpawnQuestItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation")
	TArray<FVector> ZoneLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation")
	TArray<FVector> SpawnerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation | QuestItem_1")
	FVector GunpowderItemLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation | QuestItem_2")
	FVector PlasticItemLocation;

	virtual void BeginPlay() override;

};
