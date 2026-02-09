// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseItem::ABaseItem()
{
	//추후 필요할 수도 있음
	PrimaryActorTick.bCanEverTick = false;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(100.f); // 반지름 설정
}

void ABaseItem::Interact(AActor* Interactor)
{
	// 상호작용하는 액터가 유효한지 확인
	if (!Interactor)
	{
		return;
	}

	// 상호작용하는 액터의 인벤토리 컴포넌트 가져오기
	UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();

	if (Inventory)
	{
		int32 Leftover = Inventory->AddItem(ItemID, Quantity);

		if (Leftover <= 0)
		{
			Destroy();
		}
		else
		{
			Quantity = Leftover;
			UE_LOG(LogTemp, Warning, TEXT("Not all items were added to inventory. Leftover quantity: %d"), Leftover);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactor has no InventoryComponent"));
	}
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	// 게임 인스턴스에서 아이템 데이터 서브시스템 가져오기
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnConstruction for ItemID: %s"), *ItemID.ToString());
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();

	// 아이템 ID에 해당하는 아이템 데이터 가져와서 메쉬 설정
	if (ItemDataSubsystem)
	{
		if (!ItemID.IsNone())
		{
			FItemData Data = ItemDataSubsystem->GetItemDataByID(ItemID);

			if (Data.PickupMesh.IsValid())
			{
				UStaticMesh* Mesh = Data.PickupMesh.LoadSynchronous();
				if (Mesh)
				{
					ItemMesh->SetStaticMesh(Mesh);
					UE_LOG(LogTemp, Warning, TEXT("Item Mesh set for ItemID: %s"), *ItemID.ToString());
				}
			}
		}
	}
}



