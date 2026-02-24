// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/InteractionInterface.h"
#include "UI/Item/LootTagWidget.h"

// Sets default values
ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(100.f); // 반지름 설정

	LootWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LootWidget"));
	LootWidget->SetupAttachment(RootComponent);
	LootWidget->SetWidgetSpace(EWidgetSpace::World);
	LootWidget->SetVisibility(false);
	LootWidget->SetBlendMode(EWidgetBlendMode::Masked);

	LootWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));

	LootWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LootWidget->SetCastShadow(false);


}

void ABaseItem::Interact_Implementation(AActor* Interactor)
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

void ABaseItem::SetInteractFocus_Implementation(bool bIsFocus)
{

}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	if (ItemMesh)
	{
		ItemMesh->SetStaticMesh(nullptr);
	}
	if (ItemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID is None in BeginPlay for %s"), *GetName());
		return;
	}


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
					ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					ItemMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
					ItemMesh->SetSimulatePhysics(true);
					UE_LOG(LogTemp, Warning, TEXT("Item Mesh set for ItemID: %s"), *ItemID.ToString());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh for ItemID: %s"), *ItemID.ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No valid PickupMesh for ItemID: %s"), *ItemID.ToString());
			}
		}
	}


	if (LootWidget)
	{
		LootWidget->InitWidget();


		UUserWidget* UserWidget = LootWidget->GetUserWidgetObject();
		if (UserWidget)
		{
			ULootTagWidget* LootTagWidget = Cast<ULootTagWidget>(UserWidget);
			if (LootTagWidget)
			{
				LootTagWidget->InitLootTag(ItemID, Quantity);
				LootWidget->SetDrawAtDesiredSize(true);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LootWidget has no UserWidget object in BeginPlay for ItemID: %s"), *ItemID.ToString());
		}
	}
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LootWidget && LootWidget->IsVisible())
	{
		UpdateLootWidgetTransform();
	}
}

void ABaseItem::SetItemFocus(bool bIsFocus)
{
	if (LootWidget)
	{
		if (bIsFocus)
		{
			UpdateLootWidgetTransform();
		}
		LootWidget->SetVisibility(bIsFocus);
	}

	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bIsFocus);

		if (bIsFocus)
		{
			ItemMesh->SetCustomDepthStencilValue(1); // 스텐실 값 설정 (예: 252)
		}
		else
		{
			ItemMesh->SetCustomDepthStencilValue(0); // 스텐실 값 초기화
		}
	}
}

void ABaseItem::UpdateLootWidgetTransform()
{
	if (!LootWidget || !ItemMesh)
	{
		return;
	}
	
	float MeshTopZ = ItemMesh->Bounds.BoxExtent.Z;

	FVector TargetLocation = ItemMesh->Bounds.Origin + FVector(0.f, 0.f, MeshTopZ);
	LootWidget->SetWorldLocation(TargetLocation);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		FVector CameraLocation = CameraManager->GetCameraLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, CameraLocation);
		FRotator YawOnlyRotation(0.f, LookAtRotation.Yaw, 0.f);
		LootWidget->SetWorldRotation(YawOnlyRotation);
	}
}



