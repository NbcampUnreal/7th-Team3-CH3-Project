// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/ItemTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

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

	void InitializeQuickSlots();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AssignToQuickSlot(int32 SlotIndex, FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItemFromQuickSlot(int32 SlotIndex);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnQuickSlotUpdated OnQuickSlotUpdated;

	int32 GetCapacity() const { return Capacity; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventoryItem> InventoryContents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Capacity = 20;

private:
	AWeaponItem* FindEquippedWeapon() const;

	bool HandleWeaponEquip(const FItemData& Data, FName ItemID);
	bool HandleAttachmentEquip(const FItemData& Data, FName ItemID);
	bool HandleArmorEquip(const FItemData& Data, FName ItemID);
	bool HandleConsumableUse(const FItemData& Data, FName ItemID);
};
