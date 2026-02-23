// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/ItemTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, bool, bIsEquipping, FName, ItemID);

//테스트용 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipRequested, FInventoryItem, EquipItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnequipRequested, ESlotType, SlotType);

class AWeaponItem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM3_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(FName ItemID, int32 Quantity);

	int32 AddItem(FName ItemID, int32 Quantity, const TMap<EAttachmentType, FName>& InAttachments);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RequestUseItem(FName ItemID);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItem(FName ItemID, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FInventoryItem>& GetInventoryContents() const { return InventoryContents; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetItemQuantity(FName ItemID) const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FName> QuickSlots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AssignToQuickSlot(int32 SlotIndex, FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItemFromQuickSlot(int32 SlotIndex);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnQuickSlotUpdated OnQuickSlotUpdated;

	int32 GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AWeaponItem* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FName GetEquippedArmorID() const { return EquippedArmorID; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetEquippedWeapon(AWeaponItem* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetEquippedArmor(FName NewArmorID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwapInventoryItems(int32 IndexA, int32 IndexB);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipAttachmentFromSlot(int32 InventoryIndex, EAttachmentType TargetSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipAttachmentToSlot(EAttachmentType AttachmentSlot, int32 TargetInventoryIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipAttachment(EAttachmentType AttachmentSlot);



	UPROPERTY(BlueprintAssignable, Category = "Inventory Event")
	FOnEquipmentChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Event")
	FOnEquipmentChanged OnArmorChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItemFromInventory(int32 InventoryIndex, ESlotType TargetSlotType);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipItemToInventory(ESlotType SourceSlotType, int32 TargetInventoryIndex);

	UPROPERTY(BlueprintAssignable, Category = "Inventory Event")
	FOnItemEquipRequested OnItemEquipRequested;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Event")
	FOnItemUnequipRequested OnItemUnequipRequested;


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(FName ItemID, int32 Quantity, int32 InventoryIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortInventory();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventoryItem> InventoryContents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Capacity = 20;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	AWeaponItem* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FName EquippedArmorID;

private:

	bool HandleAttachmentEquip(const FItemData& Data, FName ItemID);
	bool HandleConsumableUse(const FItemData& Data, FName ItemID);
};
