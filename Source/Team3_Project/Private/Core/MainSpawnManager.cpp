// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainSpawnManager.h"
#include "Core/EventZone.h"
#include "Core/EnemySpawner.h"

// Sets default values
AMainSpawnManager::AMainSpawnManager() : 
	EnemySpawnerClass(nullptr),
	EventZoneClass(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AMainSpawnManager::SpawnEventZone()
{
	if (ZoneLocation.IsValidIndex(-1)) return;

	if (EventZoneClass == nullptr) return;

	for (int i = 0; i < ZoneLocation.Num(); i++)
	{
		SpawnedZones.Add(GetWorld()->SpawnActor<AEventZone>(
			EventZoneClass,
			ZoneLocation[i],
			FRotator::ZeroRotator
		));
	}
}

void AMainSpawnManager::SpawnEnemySpawner()
{
	if (SpawnerLocation.IsValidIndex(-1)) return;

	if (EnemySpawnerClass == nullptr) return;

	for (int i = 0; i < SpawnerLocation.Num(); i++)
	{
		SpawnedEnemySpawners.Add(GetWorld()->SpawnActor<AEnemySpawner>(
			EnemySpawnerClass,
			SpawnerLocation[i],
			FRotator::ZeroRotator
		));
	}
}

void AMainSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnEventZone();
	SpawnEnemySpawner();
}
