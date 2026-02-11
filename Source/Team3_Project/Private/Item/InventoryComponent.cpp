// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/WeaponItem.h"
#include "GameFramework/Character.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	return AddItem(ItemID, Quantity, TMap<EAttachmentType, FName>());
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Quantity, const TMap<EAttachmentType, FName>& InAttachments)
{
	/* 
	반환값에 대하여
	0 : 모든 아이템이 성공적으로 추가됨 (true)
	0 < n < Quantity : 추가하지 못한 아이템의 수량(이 수량으로 바닥의 아이템 수를 업데이트)
	Quantity : 아무것도 추가하지 못함	 (false)
	*/

	// 입력 유효성 검사
	if (Quantity <= 0 || ItemID.IsNone())
	{
		return Quantity;
	}
	// 게임 인스턴스에서 아이템 데이터 서브시스템 가져오기
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return Quantity;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return Quantity;
	}

	// 아이템 데이터 가져오기
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);

	// 최대 스택 크기 확인
	int32 MaxStack = ItemData.MaxStack;
	if (MaxStack <= 0) MaxStack = 1;

	int32 RemainingQuantity = Quantity;

	// 스택이 가능한 아이템인 경우 기존 스택에 추가
	if (MaxStack > 1)
	{
		for (FInventoryItem& InventoryItem : InventoryContents)
		{
			if (InventoryItem.ItemID == ItemID)
			{
				int32 AvailableSpace = MaxStack - InventoryItem.Quantity;
				if (AvailableSpace > 0)
				{
					int32 ToAdd = FMath::Min(AvailableSpace, RemainingQuantity);
					InventoryItem.Quantity += ToAdd;
					RemainingQuantity -= ToAdd;
					if (RemainingQuantity <= 0)
					{
						OnInventoryUpdated.Broadcast();
						return 0;
					}
				}
			}
		}
	}

	// 남은 수량을 새로운 스택으로 추가
	while (RemainingQuantity > 0)
	{
		if (InventoryContents.Num() >= Capacity)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
			break;
		}
		int32 ToAdd = FMath::Min(MaxStack, RemainingQuantity);

		FInventoryItem NewItem;
		NewItem.ItemID = ItemID;
		NewItem.Quantity = ToAdd;

		NewItem.AttachmentState = InAttachments;

		InventoryContents.Add(NewItem);
		RemainingQuantity -= ToAdd;
	}

	// 인벤토리가 변경되었음을 알림
	if (RemainingQuantity < Quantity)
	{
		OnInventoryUpdated.Broadcast();
	}

	return RemainingQuantity;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	// 입력 유효성 검사
	if (!HasItem(ItemID, Quantity))
	{
		return false;
	}	

	int32 RemainingToRemove = Quantity;
	// 인벤토리에서 해당 아이템 제거
	// 뒤에서부터 제거하여 인덱스 문제 방지
	for (int32 i = InventoryContents.Num() - 1; i >= 0 && RemainingToRemove > 0; --i)
	{
		if (InventoryContents[i].ItemID == ItemID)
		{
			int32 AmountInSlot = InventoryContents[i].Quantity;
			int32 AmountTaken = FMath::Min(AmountInSlot, RemainingToRemove);

			InventoryContents[i].Quantity -= AmountTaken;
			RemainingToRemove -= AmountTaken;

			if (InventoryContents[i].Quantity <= 0)
			{
				InventoryContents.RemoveAt(i);
			}

			if (RemainingToRemove <= 0)
			{
				break;
			}
		}
	}

	// 인벤토리가 변경되었음을 알림
	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RequestUseItem(FName ItemID)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return false;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return false;
	}
	// 아이템 데이터 가져오기
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);

	// 아이템 타입에 따라 처리 분기
	switch (ItemData.ItemType)
	{
	case EItemType::IT_Weapon:
		return HandleWeaponEquip(ItemData, ItemID);
	case EItemType::IT_Attachment:
		return HandleAttachmentEquip(ItemData, ItemID);
	case EItemType::IT_Armor:
		return HandleArmorEquip(ItemData, ItemID);
	case EItemType::IT_Consumable:
		return HandleConsumableUse(ItemData, ItemID);
	default:
		return false;
	}
}

// 조합 시스템 등에서 아이템 존재 여부와 수량을 확인할 때 사용
bool UInventoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
	
	// 입력 유효성 검사
	if (ItemID.IsNone() || Quantity <= 0)
	{
		return false;
	}

	int32 TotalQuantity = 0;
	// 인벤토리에서 해당 아이템의 총 수량 계산
	for (const FInventoryItem& InventoryItem : InventoryContents)
	{
		if (InventoryItem.ItemID == ItemID)
		{
			TotalQuantity += InventoryItem.Quantity;
		}
	}

	// 해당 아이템의 총 수량이 필요한 수량 이상인지 확인
	return TotalQuantity >= Quantity;
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	// 입력 유효성 검사
	if (ItemID.IsNone())
	{
		return 0;
	}
	int32 TotalQuantity = 0;

	// 인벤토리에서 해당 아이템의 총 수량 계산
	for (const FInventoryItem& InventoryItem : InventoryContents)
	{
		if (InventoryItem.ItemID == ItemID)
		{
			TotalQuantity += InventoryItem.Quantity;
		}
	}
	return TotalQuantity;
}

AWeaponItem* UInventoryComponent::FindEquippedWeapon() const
{
	// 소유자 액터 가져오기
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}
	// 부착된 액터들 중에서 무기 아이템 찾기
	TArray<AActor*> AttachedActors;
	OwnerActor->GetAttachedActors(AttachedActors);
		for (AActor* AttachedActor : AttachedActors)
	{
		AWeaponItem* Weapon = Cast<AWeaponItem>(AttachedActor);
		if (Weapon && !Weapon->IsHidden())
		{
			return Weapon;
		}
	}
	return nullptr;
}






bool UInventoryComponent::HandleWeaponEquip(const FItemData& Data, FName ItemID)
{
	// 소유자 캐릭터 가져오기
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return false;
	}
	// 인벤토리에서 아이템의 부착 상태 가져오기
	TMap<EAttachmentType, FName> SavedAttachments;
	bool bFoundData = false;

	for (const FInventoryItem& InventoryItem : InventoryContents)
	{
		if (InventoryItem.ItemID == ItemID)
		{
			SavedAttachments = InventoryItem.AttachmentState;
			bFoundData = true;
			break;
		}
	}

	if (!bFoundData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s not found in inventory"), *ItemID.ToString());
		return false;
	}

	// 기존에 장착된 무기 제거 및 인벤토리에 추가
	AWeaponItem* OldWeapon = FindEquippedWeapon();

	if (OldWeapon)
	{
		OldWeapon->StopReload();	
		TMap<EAttachmentType, FName> OldAttachments = OldWeapon->GetAttachmentState();
		FName OldWeaponID = OldWeapon->GetItemID();

		AddItem(OldWeaponID, 1, OldAttachments);
		OldWeapon->Destroy();
	}

	if (!RemoveItem(ItemID, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to remove ItemID %s from inventory"), *ItemID.ToString());
		return false;
	}

	// 무기 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Data.ItemClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClass is null for ItemID %s"), *ItemID.ToString());
		return false;
	}

	AWeaponItem* NewWeapon = GetWorld()->SpawnActor<AWeaponItem>(
		Data.ItemClass.LoadSynchronous(),
		Character->GetActorTransform(),
		SpawnParams
	);

	// 무기 장착
	if (NewWeapon)
	{
		NewWeapon->AttachToComponent(
			Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			FName("WeaponSocket")
		);

		NewWeapon->SetItemID(ItemID);
		NewWeapon->ApplyAttachmentState(SavedAttachments);
		UE_LOG(LogTemp, Log, TEXT("Equipped weapon ItemID %s"), *ItemID.ToString());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to spawn weapon for ItemID %s"), *ItemID.ToString());
	return false;
}



bool UInventoryComponent::HandleAttachmentEquip(const FItemData& Data, FName ItemID)
{
	// 장착된 무기 찾기
	AWeaponItem* Weapon = FindEquippedWeapon();
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped to attach ItemID %s"), *ItemID.ToString());
		return false;
	}

	// 인벤토리에서 아이템 제거 및 무기에 부착
	if (RemoveItem(ItemID, 1))
	{
		FName RemovedAttachmentID = Weapon->EquipAttachment(ItemID);

		if (!RemovedAttachmentID.IsNone())
		{
			// 이전에 장착된 부착물이 있다면 인벤토리에 추가
			AddItem(RemovedAttachmentID, 1);
		}
		UE_LOG(LogTemp, Log, TEXT("Equipped attachment ItemID %s"), *ItemID.ToString());
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to remove attachment ItemID %s from inventory"), *ItemID.ToString());
	return false;
}



bool UInventoryComponent::HandleArmorEquip(const FItemData& Data, FName ItemID)
{
	//캐릭터 상황 봐서 추후 구현
	//ACharacter* Character = Cast<ACharacter>(GetOwner());
	//if (!Character)
	//{
	//	return false;
	//}

	//if (RemoveItem(ItemID, 1))
	//{
	//	// 방어구 장착 로직 구현 필요
	//	UE_LOG(LogTemp, Log, TEXT("Equipped armor ItemID %s"), *ItemID.ToString());
	//	return true;
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Failed to remove armor ItemID %s from inventory"), *ItemID.ToString());
	return false;
}



bool UInventoryComponent::HandleConsumableUse(const FItemData& Data, FName ItemID)
{
	//캐릭터 상황 봐서 회복 로직 구현
	//Character

	if (RemoveItem(ItemID, 1))
		{
		UE_LOG(LogTemp, Log, TEXT("Used consumable ItemID %s"), *ItemID.ToString());
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to remove consumable ItemID %s from inventory"), *ItemID.ToString());
	return false;
}
