// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/InteractionInterface.h"

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

	LootWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LootWidget"));
	LootWidget->SetupAttachment(RootComponent);
	LootWidget->SetWidgetSpace(EWidgetSpace::Screen);
	LootWidget->SetDrawAtDesiredSize(true);
	LootWidget->SetVisibility(false);

	LootWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereEndOverlap);
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

FText ABaseItem::GetInteractionPrompt_Implementation()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return FText::GetEmpty();
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (ItemDataSubsystem)
	{
		FItemData Data = ItemDataSubsystem->GetItemDataByID(ItemID);
		return FText::Format(NSLOCTEXT("Interaction", "PickupPrompt", "Press E to pick up {0}"), Data.DisplayName);
	}
	else 
	{
		return FText::GetEmpty();
	}
}

void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor->ActorHasTag("Player")))
	{
		SetItemFocus(true);
	}
}

void ABaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor->ActorHasTag("Player")))
	{
		SetItemFocus(false);
	}
}

void ABaseItem::SetItemFocus(bool bIsFocus)
{
	if (LootWidget)
	{
		LootWidget->SetVisibility(bIsFocus);
	}

	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bIsFocus);
	}
}



