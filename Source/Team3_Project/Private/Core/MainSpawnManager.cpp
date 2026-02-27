// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainSpawnManager.h"
#include "Core/EventZone.h"
#include "Core/MainGameState.h"
#include "Core/EnemySpawner.h"
#include "Core/InteractDoorWidget.h"
#include "Core/DoorNPC.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainSpawnManager::AMainSpawnManager() :
	EnemySpawnerClass(nullptr),
	EventZoneClass(nullptr)
{

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

void AMainSpawnManager::SpawnQuestItem()
{
	if (FirstQuestItemLocation.IsValidIndex(-1) || SecondQuestItemLocation.IsValidIndex(-1)) return;

	//if (SpawnQuestItemClass == nullptr) return;

	//TODO_@Core : NPC가 대화창열렸다는 신호 수신시 델리게이트 받아서 아이템 스폰하기 1번만,

	/*GetWorld()->SpawnActor<AQuestItem>(
		QuestItemClass1,
		FirstQuestItemLocation,
		FRotator::ZeroRotator
	);

	GetWorld()->SpawnActor<AQuestItem>(
		QuestItemClass2,
		SecondQuestItemLocation,
		FRotator::ZeroRotator
	);*/

}

void AMainSpawnManager::BindSpawnQuestItem()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoorNPC::StaticClass(), Found);

	if (Found.IsEmpty())
	{
		return;
	}

	for (AActor* Actor : Found)
	{
		ADoorNPC* DoorNpcActor = Cast<ADoorNPC>(Actor);
		DoorNpcActor->OnInteractDoor.AddDynamic(this, &AMainSpawnManager::SpawnQuestItem);
	}
}

void AMainSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	BindSpawnQuestItem();
	SpawnEventZone();
	SpawnEnemySpawner();
	SpawnQuestItem();
}
