// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainSpawnManager.h"
#include "Core/EventZone.h"
#include "Core/MainGameState.h"
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

	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	for (int i = 0; i < ZoneLocation.Num(); i++)
	{
		const int32 SpawnerId = i + 1;

		AEventZone* EventZoneInstance = GetWorld()->SpawnActor<AEventZone>(
			EventZoneClass,
			ZoneLocation[i],
			FRotator::ZeroRotator
		);

		EventZoneInstance->InitId(SpawnerId);
		CurrentGameState->FindEventZone(SpawnerId, EventZoneInstance);
	}
}

void AMainSpawnManager::SpawnEnemySpawner()
{
	if (SpawnerLocation.IsValidIndex(-1)) return;

	if (EnemySpawnerClass == nullptr) return;

	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());
	
	for (int i = 0; i < SpawnerLocation.Num(); i++)
	{
		const int32 SpawnerId = i + 1;

		AEnemySpawner* SpawnerInstance = GetWorld()->SpawnActor<AEnemySpawner>(
			EnemySpawnerClass,
			SpawnerLocation[i],
			FRotator::ZeroRotator
		);

		SpawnerInstance->InitId(SpawnerId);
		CurrentGameState->FindSpawner(SpawnerId, SpawnerInstance);
	}
}

void AMainSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnEventZone();
	SpawnEnemySpawner();
}
