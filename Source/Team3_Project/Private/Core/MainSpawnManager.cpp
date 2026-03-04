// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainSpawnManager.h"
#include "Core/EventZone.h"
#include "Core/MainGameState.h"
#include "Core/EnemySpawner.h"
#include "Core/InteractDoorWidget.h"
#include "Core/DoorNPC.h"
#include "Core/MainGameInstance.h"
#include "Core/ItemDataSubsystem.h"
#include "Core/DebugHelper.h"
#include "Item/BaseItem.h"
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
	if (ZoneLocation.IsEmpty()) return;

	if (EventZoneClass == nullptr) return;

	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());
	if (!CurrentGameState) return;

	for (int i = 0; i < ZoneLocation.Num(); i++)
	{
		const int32 SpawnerId = i + 1;

		AEventZone* EventZoneInstance = GetWorld()->SpawnActor<AEventZone>(
			EventZoneClass,
			ZoneLocation[i],
			FRotator::ZeroRotator
		);

		if (EventZoneInstance)
		{
		EventZoneInstance->InitId(SpawnerId);
		CurrentGameState->FindEventZone(SpawnerId, EventZoneInstance);
		}
	}
}

void AMainSpawnManager::SpawnEnemySpawner()
{
	if (SpawnerLocation.IsEmpty()) return;

	if (EnemySpawnerClass == nullptr) return;

	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());
	
	if (!CurrentGameState) return;

	for (int i = 0; i < SpawnerLocation.Num(); i++)
	{
		const int32 SpawnerId = i + 1;

		AEnemySpawner* SpawnerInstance = GetWorld()->SpawnActor<AEnemySpawner>(
			EnemySpawnerClass,
			SpawnerLocation[i],
			FRotator::ZeroRotator
		);

		if (SpawnerInstance)
		{
			SpawnerInstance->InitId(SpawnerId);
			CurrentGameState->FindSpawner(SpawnerId, SpawnerInstance);
		}
	}
}

void AMainSpawnManager::SpawnQuestItem()
{
	UGameInstance* CurrentGameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (CurrentGameInstance == nullptr)
	{
		Debug::Print(TEXT("no Gameinstance"));
		return;
	}

	UItemDataSubsystem* ItemDataSubSystem = CurrentGameInstance->GetSubsystem<UItemDataSubsystem>();
	if (ItemDataSubSystem == nullptr)
	{
		Debug::Print(TEXT("no ItemDataSubSystem"));
		return;
	}


	FItemData GunpowderItemData = ItemDataSubSystem->GetItemDataByID("Gunpowder");
	FItemData PlasticItemData = ItemDataSubSystem->GetItemDataByID("Plastic");

	UClass* GunpowderItemClass = GunpowderItemData.ItemClass.LoadSynchronous();
	UClass* PlasticItemClass = PlasticItemData.ItemClass.LoadSynchronous();

	if (GunpowderItemClass && PlasticItemClass)
	{
		AActor* OwnerActor = GetOwner();

		FTransform GunpowderTransform(FRotator::ZeroRotator, GunpowderItemLocation);
		FTransform PlasticTransform(FRotator::ZeroRotator, PlasticItemLocation);

		ABaseItem* SpawnGunpowder = GetWorld()->SpawnActorDeferred<ABaseItem>(
			GunpowderItemClass,
			GunpowderTransform,
			nullptr, // 소유자
			Cast<APawn>(OwnerActor), // 누가 유발했나
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		ABaseItem* SpawnPlastic = GetWorld()->SpawnActorDeferred<ABaseItem>(
			PlasticItemClass,
			PlasticTransform,
			nullptr, // 소유자
			Cast<APawn>(OwnerActor), // 누가 유발했나
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		if (SpawnGunpowder)
		{
			SpawnGunpowder->SetItemID("Gunpowder");
			SpawnGunpowder->SetQuantity(1);
			SpawnGunpowder->FinishSpawning(GunpowderTransform);
		}

		if (SpawnPlastic)
		{
			SpawnPlastic->SetItemID("Plastic");
			SpawnPlastic->SetQuantity(1);
			SpawnPlastic->FinishSpawning(PlasticTransform);
		}
	}
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
}
