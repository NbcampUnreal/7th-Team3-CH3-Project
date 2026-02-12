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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectClass")
	TSubclassOf<AEventZone> EventZoneClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectClass")
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnedObjects")
	TArray<AEventZone*> SpawnedZones;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnedObjects")
	TArray<AEnemySpawner*> SpawnedEnemySpawners;

	void SpawnEventZone();
	void SpawnEnemySpawner();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation")
	TArray<FVector> ZoneLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnLocation")
	TArray<FVector> SpawnerLocation;

	virtual void BeginPlay() override;

};
