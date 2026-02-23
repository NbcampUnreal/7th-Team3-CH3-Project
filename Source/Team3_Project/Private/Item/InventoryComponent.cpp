// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/WeaponItem.h"
#include "GameFramework/Character.h"
#include "Shared/Component/StatComponent.h"
#include "Shared/EquipableInterface.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	QuickSlots.Init(NAME_None, 8);

	FInventoryItem EmptyItem;
	EmptyItem.ItemID = NAME_None;
	EmptyItem.Quantity = 0;

	InventoryContents.Init(EmptyItem, Capacity);
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
			if (!InventoryItem.ItemID.IsNone() && InventoryItem.ItemID == ItemID)
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
		int32 EmptyIndex = INDEX_NONE;

		for (int32 i = 0; i < InventoryContents.Num(); ++i)
		{
			if (InventoryContents[i].ItemID.IsNone())
			{
				EmptyIndex = i;
				break;
			}
		}

		if (EmptyIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
			break;
		}

		int32 ToAdd = FMath::Min(MaxStack, RemainingQuantity);

		InventoryContents[EmptyIndex].ItemID = ItemID;
		InventoryContents[EmptyIndex].Quantity = ToAdd;
		InventoryContents[EmptyIndex].AttachmentState = InAttachments;

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
		if (!InventoryContents[i].ItemID.IsNone() && InventoryContents[i].ItemID == ItemID)
		{
			int32 AmountInSlot = InventoryContents[i].Quantity;
			int32 AmountTaken = FMath::Min(AmountInSlot, RemainingToRemove);

			InventoryContents[i].Quantity -= AmountTaken;
			RemainingToRemove -= AmountTaken;

			if (InventoryContents[i].Quantity <= 0)
			{
				InventoryContents[i].ItemID = NAME_None;
				InventoryContents[i].AttachmentState.Empty();
				InventoryContents[i].Quantity = 0;
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

	// 요청된 아이템이 인벤토리에 존재하는지 확인
	int32 FoundIndex = INDEX_NONE;
	for (int32 i = 0; i < InventoryContents.Num(); ++i)
	{
		if (InventoryContents[i].ItemID == ItemID)
		{
			FoundIndex = i;
			break;
		}
	}

	if (FoundIndex == INDEX_NONE && (ItemData.ItemType == EItemType::IT_Weapon || ItemData.ItemType == EItemType::IT_Armor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s not found in inventory for use request"), *ItemID.ToString());
		return false;
	}

	// 아이템 타입에 따라 처리 분기
	switch (ItemData.ItemType)
	{
	case EItemType::IT_Weapon:
		EquipItemFromInventory(FoundIndex, ESlotType::ST_Weapon);
		return true;
	case EItemType::IT_Attachment:
		return HandleAttachmentEquip(ItemData, ItemID);
	case EItemType::IT_Armor:
		EquipItemFromInventory(FoundIndex, ESlotType::ST_Armor);
		return true;
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

void UInventoryComponent::AssignToQuickSlot(int32 SlotIndex, FName ItemID)
{
	if (QuickSlots.IsValidIndex(SlotIndex))
	{
		QuickSlots[SlotIndex] = ItemID;
		OnQuickSlotUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Assigned ItemID %s to Quick Slot %d"), *ItemID.ToString(), SlotIndex);
	}
}

void UInventoryComponent::UseItemFromQuickSlot(int32 SlotIndex)
{
	if (!QuickSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Quick Slot Index: %d"), SlotIndex);
		return;
	}

	FName TargetItemID = QuickSlots[SlotIndex];
	if (TargetItemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Quick Slot %d is empty"), SlotIndex);
		return;
	}

	if (HasItem(TargetItemID, 1))
	{
		if (RequestUseItem(TargetItemID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Used ItemID %s from Quick Slot %d"), *TargetItemID.ToString(), SlotIndex);
			OnQuickSlotUpdated.Broadcast();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to use ItemID %s from Quick Slot %d"), *TargetItemID.ToString(), SlotIndex);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough quantity of ItemID %s to use from Quick Slot %d"), *TargetItemID.ToString(), SlotIndex);
	}

}

void UInventoryComponent::SetEquippedWeapon(AWeaponItem* NewWeapon)
{
	EquippedWeapon = NewWeapon;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SetEquippedArmor(FName NewArmorID)
{
	EquippedArmorID = NewArmorID;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SwapInventoryItems(int32 IndexA, int32 IndexB)
{
	if (IndexA == IndexB)
	{
		return;
	}
	if (!InventoryContents.IsValidIndex(IndexA) || !InventoryContents.IsValidIndex(IndexB))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory indices for swapping: %d and %d"), IndexA, IndexB);
		return;
	}
	InventoryContents.Swap(IndexA, IndexB);
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::EquipAttachmentFromSlot(int32 InventoryIndex, EAttachmentType TargetSlot)
{
	if (!InventoryContents.IsValidIndex(InventoryIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory index for equipping attachment: %d"), InventoryIndex);
		return;
	}

	FInventoryItem& Item = InventoryContents[InventoryIndex];
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(Item.ItemID);

	if (ItemData.ItemType != EItemType::IT_Attachment || ItemData.AttachmentType != TargetSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s is not an attachment and cannot be equipped"), *Item.ItemID.ToString());
		return;
	}

	if (RequestUseItem(Item.ItemID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipped attachment ItemID %s to slot %d"), *Item.ItemID.ToString(), static_cast<int32>(TargetSlot));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to equip attachment ItemID %s to slot %d"), *Item.ItemID.ToString(), static_cast<int32>(TargetSlot));
	}
}

void UInventoryComponent::UnequipAttachmentToSlot(EAttachmentType AttachmentSlot, int32 TargetInventoryIndex)
{
	if (!InventoryContents.IsValidIndex(TargetInventoryIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory index for unequipping attachment: %d"), TargetInventoryIndex);
		return;
	}

	if (TargetInventoryIndex >= InventoryContents.Num())
	{
		if (InventoryContents.Num() >= Capacity)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot unequip attachment to inventory index %d because inventory is full"), TargetInventoryIndex);
			return;
		}
		TargetInventoryIndex = InventoryContents.Num();
	}
	if (TargetInventoryIndex == InventoryContents.Num())
	{
		InventoryContents.AddDefaulted();
	}

	AWeaponItem* Weapon = GetEquippedWeapon();
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped to unequip attachment from slot %d"), static_cast<int32>(AttachmentSlot));
		return;
	}

	const TMap<EAttachmentType, FName>& Attachments = Weapon->GetAttachmentState();
	if (!Attachments.Contains(AttachmentSlot) || Attachments[AttachmentSlot].IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("No attachment equipped in slot %d to unequip"), static_cast<int32>(AttachmentSlot));
		return;
	}

	FName CurrentAttachmentID = Attachments[AttachmentSlot];

	FInventoryItem& TargetSlotItem = InventoryContents[TargetInventoryIndex];

	if (TargetSlotItem.ItemID.IsNone())
	{
		Weapon->UnequipAttachment(CurrentAttachmentID);

		TargetSlotItem.ItemID = CurrentAttachmentID;
		TargetSlotItem.Quantity = 1;

		OnInventoryUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Unequipped attachment ItemID %s from slot %d to inventory index %d"), *CurrentAttachmentID.ToString(), static_cast<int32>(AttachmentSlot), TargetInventoryIndex);
		return;
	}

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}

	FItemData TargetItemData = ItemDataSubsystem->GetItemDataByID(TargetSlotItem.ItemID);

	if (TargetItemData.ItemType == EItemType::IT_Attachment && TargetItemData.AttachmentType == AttachmentSlot)
	{
		Weapon->UnequipAttachment(CurrentAttachmentID);
		Weapon->EquipAttachment(TargetSlotItem.ItemID);
		TargetSlotItem.ItemID = CurrentAttachmentID;
		TargetSlotItem.Quantity = 1;
		OnInventoryUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Swapped attachment ItemID %s in slot %d with inventory index %d"), *CurrentAttachmentID.ToString(), static_cast<int32>(AttachmentSlot), TargetInventoryIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory item at index %d is not compatible for swapping with attachment slot %d"), TargetInventoryIndex, static_cast<int32>(AttachmentSlot));
	}
}

void UInventoryComponent::UnequipAttachment(EAttachmentType AttachmentSlot)
{
	AWeaponItem* Weapon = GetEquippedWeapon();
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped to unequip attachment from slot %d"), static_cast<int32>(AttachmentSlot));
		return;
	}

	const TMap<EAttachmentType, FName>& Attachments = Weapon->GetAttachmentState();
	if (!Attachments.Contains(AttachmentSlot) || Attachments[AttachmentSlot].IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("No attachment equipped in slot %d to unequip"), static_cast<int32>(AttachmentSlot));
		return;
	}

	FName CurrentAttachmentID = Attachments[AttachmentSlot];
	if (AddItem(CurrentAttachmentID, 1) < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to add unequipped attachment ItemID %s back to inventory"), *CurrentAttachmentID.ToString());
		return;
	}

	Weapon->UnequipAttachment(CurrentAttachmentID);
	UE_LOG(LogTemp, Warning, TEXT("Unequipped attachment ItemID %s from slot %d"), *CurrentAttachmentID.ToString(), static_cast<int32>(AttachmentSlot));
}

void UInventoryComponent::EquipItemFromInventory(int32 InventoryIndex, ESlotType TargetSlotType)
{
	if (!InventoryContents.IsValidIndex(InventoryIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory index for equipping item: %d"), InventoryIndex);
		return;
	}

	FInventoryItem ItemToEquip = InventoryContents[InventoryIndex];
	FInventoryItem OldItem;

	if (IEquipableInterface* EquipableCharacter = Cast<IEquipableInterface>(GetOwner()))
	{
		OldItem = EquipableCharacter->UnequipItemBySlot(TargetSlotType);
		EquipableCharacter->EquipItemByData(ItemToEquip, TargetSlotType);
	}
	else
	{ // 테스트용 델리게이트 로직 - 실제 게임에서는 EquipableInterface을 구현한 캐릭터에서 처리되어야 함
		if (TargetSlotType == ESlotType::ST_Weapon)
		{
			if (EquippedWeapon)
			{
				OldItem.ItemID = EquippedWeapon->GetItemID();
				OldItem.Quantity = 1;
				OldItem.AttachmentState = EquippedWeapon->GetAttachmentState();
			}
		}
		else if (TargetSlotType == ESlotType::ST_Armor)
		{
			if (!EquippedArmorID.IsNone())
			{
				OldItem.ItemID = EquippedArmorID;
				OldItem.Quantity = 1;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid target slot type for equipping item: %d"), static_cast<int32>(TargetSlotType));
			return;
		}
		OnItemEquipRequested.Broadcast(ItemToEquip);
	}


	if (!OldItem.ItemID.IsNone())
	{
		InventoryContents[InventoryIndex] = OldItem;
		OnInventoryUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Swapped equipped item in slot %d with inventory index %d"), static_cast<int32>(TargetSlotType), InventoryIndex);
	}
	else
	{
		InventoryContents.RemoveAt(InventoryIndex);
	}


	if (TargetSlotType == ESlotType::ST_Armor)
	{
		EquippedArmorID = ItemToEquip.ItemID;
	}
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::UnequipItemToInventory(ESlotType SourceSlotType, int32 TargetInventoryIndex)
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}

	if (TargetInventoryIndex >= InventoryContents.Num())
	{
		if (InventoryContents.Num() >= Capacity)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory is full, cannot unequip item from slot %d"), static_cast<int32>(SourceSlotType));
			return;
		}
		TargetInventoryIndex = InventoryContents.Num();
	}
	if (TargetInventoryIndex == InventoryContents.Num())
	{
		InventoryContents.AddDefaulted();
	}

	FInventoryItem& TargetSlotItem = InventoryContents[TargetInventoryIndex];

	if (TargetSlotItem.ItemID.IsNone())
	{
		if (IEquipableInterface* EquipableCharacter = Cast<IEquipableInterface>(GetOwner()))
		{
			FInventoryItem UnequippedItem = EquipableCharacter->UnequipItemBySlot(SourceSlotType);
			if (!UnequippedItem.ItemID.IsNone())
			{
				TargetSlotItem = UnequippedItem;
				OnInventoryUpdated.Broadcast();
				UE_LOG(LogTemp, Warning, TEXT("Unequipped item from slot %d to inventory index %d"), static_cast<int32>(SourceSlotType), TargetInventoryIndex);
			}
			else
			{
				if (TargetInventoryIndex == InventoryContents.Num() - 1)
				{
					InventoryContents.RemoveAt(TargetInventoryIndex);
				}
			}
		}
		else
		{
			if (SourceSlotType == ESlotType::ST_Weapon && EquippedWeapon)
			{
				TargetSlotItem.ItemID = EquippedWeapon->GetItemID();
				TargetSlotItem.Quantity = 1;
				TargetSlotItem.AttachmentState = EquippedWeapon->GetAttachmentState();
			}
			else if (SourceSlotType == ESlotType::ST_Armor && !EquippedArmorID.IsNone())
			{
				TargetSlotItem.ItemID = EquippedArmorID;
				TargetSlotItem.Quantity = 1;
				EquippedArmorID = NAME_None;
			}
			OnItemUnequipRequested.Broadcast(SourceSlotType);
		}
	}
	else
	{
		bool bCanSwap = false;
		FItemData TargetItemData = ItemDataSubsystem->GetItemDataByID(TargetSlotItem.ItemID);

		if (SourceSlotType == ESlotType::ST_Weapon && TargetItemData.ItemType == EItemType::IT_Weapon)
		{
			bCanSwap = true;
		}
		else if (SourceSlotType == ESlotType::ST_Armor && TargetItemData.ItemType == EItemType::IT_Armor)
		{
			bCanSwap = true;
		}

		if (bCanSwap)
		{
			UE_LOG(LogTemp, Warning, TEXT("Swapping equipped item in slot %d with inventory index %d"), static_cast<int32>(SourceSlotType), TargetInventoryIndex);
			EquipItemFromInventory(TargetInventoryIndex, SourceSlotType);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unequipped item from slot %d to new inventory index %d"), static_cast<int32>(SourceSlotType), InventoryContents.Num() - 1);
			UnequipItemToInventory(SourceSlotType, InventoryContents.Num());
		}
	}
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::DropItem(FName ItemID, int32 Quantity, int32 InventoryIndex)
{
	if (!InventoryContents.IsValidIndex(InventoryIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory index for dropping item: %d"), InventoryIndex);
		return;
	}
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}


	FInventoryItem ItemToDrop = InventoryContents[InventoryIndex];
	ItemToDrop.Quantity = Quantity;

	if (RemoveItem(ItemID, Quantity))
	{
		FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);
		UClass* SpawnClass = ItemData.ItemClass.LoadSynchronous();

		if (SpawnClass)
		{
			AActor* OwnerActor = GetOwner();
			FVector SpawnLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * 100.0f;
			FTransform SpawnTransform(OwnerActor->GetActorRotation(), SpawnLocation);

			ABaseItem* DroppedItem = GetWorld()->SpawnActorDeferred<ABaseItem>(
				SpawnClass,
				SpawnTransform,
				OwnerActor,
				Cast<APawn>(OwnerActor),
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);

			if (DroppedItem)
			{
				DroppedItem->SetItemID(ItemID);
				DroppedItem->SetQuantity(Quantity);

				if (AWeaponItem* Weapon = Cast<AWeaponItem>(DroppedItem))
				{
					Weapon->ApplyAttachmentState(ItemToDrop.AttachmentState);
				}

				DroppedItem->FinishSpawning(SpawnTransform);
			}
		}
		OnInventoryUpdated.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to drop ItemID %s with quantity %d from inventory index %d"), *ItemID.ToString(), Quantity, InventoryIndex);
	}
}

void UInventoryComponent::SortInventory()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}

	InventoryContents.Sort([ItemDataSubsystem](const FInventoryItem& A, const FInventoryItem& B)
		{
			if (A.ItemID.IsNone() && B.ItemID.IsNone())
			{
				return false; // 둘 다 빈 슬롯이면 순서 유지
			}
			if (A.ItemID.IsNone())
			{
				return false; // A가 빈 슬롯이면 B가 앞에 오도록
			}
			if (B.ItemID.IsNone())
			{
				return true; // B가 빈 슬롯이면 A가 앞에 오도록
			}

			FItemData ItemDataA = ItemDataSubsystem->GetItemDataByID(A.ItemID);
			FItemData ItemDataB = ItemDataSubsystem->GetItemDataByID(B.ItemID);

			return ItemDataA.DisplayName.ToString() < ItemDataB.DisplayName.ToString(); // 이름 기준으로 정렬
		}
	);

	OnInventoryUpdated.Broadcast();
}


bool UInventoryComponent::HandleAttachmentEquip(const FItemData& Data, FName ItemID)
{
	// 장착된 무기 찾기
	AWeaponItem* Weapon = GetEquippedWeapon();
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
		UE_LOG(LogTemp, Warning, TEXT("Equipped attachment ItemID %s"), *ItemID.ToString());
		OnInventoryUpdated.Broadcast();
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to remove attachment ItemID %s from inventory"), *ItemID.ToString());
	OnInventoryUpdated.Broadcast();
	return false;
}




bool UInventoryComponent::HandleConsumableUse(const FItemData& Data, FName ItemID)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return false;
	}

	UStatComponent* StatComp = Character->FindComponentByClass<UStatComponent>();
	if (!StatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponent not found on character for ItemID %s"), *ItemID.ToString());
		return false;
	}

	bool bUseSuccessful = false;

	switch (Data.ConsumableType)
	{
	case EConsumableType::CT_Health:
		// 체력 회복 로직 구현 필요		
		UE_LOG(LogTemp, Warning, TEXT("Used Health Consumable ItemID %s"), *ItemID.ToString());
		bUseSuccessful = true;
		break;
	case EConsumableType::CT_Stamina:
		// 스태미나 회복 로직 구현 필요
		UE_LOG(LogTemp, Warning, TEXT("Used Stamina Consumable ItemID %s"), *ItemID.ToString());
		bUseSuccessful = true;
		break;
	case EConsumableType::CT_Adrenaline:
		// 아드레날린 회복 로직 구현 필요
		UE_LOG(LogTemp, Warning, TEXT("Used Adrenaline Consumable ItemID %s"), *ItemID.ToString());
		bUseSuccessful = true;
		break;
	case EConsumableType::CT_WhiteKarma:
		// 화이트 카르마 회복 로직 구현 필요
		UE_LOG(LogTemp, Warning, TEXT("Used White Karma Consumable ItemID %s"), *ItemID.ToString());
		bUseSuccessful = true;
		break;
	case EConsumableType::CT_BlackKarma:
		// 블랙 카르마 회복 로직 구현 필요
		UE_LOG(LogTemp, Warning, TEXT("Used Black Karma Consumable ItemID %s"), *ItemID.ToString());
		bUseSuccessful = true;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown ConsumableType for ItemID %s"), *ItemID.ToString());
		bUseSuccessful = false;
		break;
	}

	if (bUseSuccessful && RemoveItem(ItemID, 1))
	{
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to remove consumable ItemID %s from inventory"), *ItemID.ToString());
	return false;
}
