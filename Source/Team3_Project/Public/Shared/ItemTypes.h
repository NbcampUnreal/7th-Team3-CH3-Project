// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

class ABaseItem;
class AWeaponItem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_None UMETA(DisplayName = "None"),
	IT_Weapon UMETA(DisplayName = "Weapon"),
	IT_Consumable UMETA(DisplayName = "Consumable"),
	IT_Armor UMETA(DisplayName = "Armor"),
	IT_Ammo UMETA(DisplayName = "Ammo"),
	IT_Attachment UMETA(DisplayName = "Attachment"),
	IT_Ingredient UMETA(DisplayName = "Ingredient"),
	IT_QuestItem UMETA(DisplayName = "Quest Item")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None UMETA(DisplayName = "None"),
	WT_Pistol UMETA(DisplayName = "Pistol"),
	WT_Rifle UMETA(DisplayName = "Rifle"),
	WT_Shotgun UMETA(DisplayName = "Shotgun"),
	WT_Sniper UMETA(DisplayName = "Sniper"),
	WT_Melee UMETA(DisplayName = "Melee")
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	AT_None UMETA(DisplayName = "None"),
	AT_Scope UMETA(DisplayName = "Scope"),
	AT_Barrel UMETA(DisplayName = "Barrel"),
	AT_Magazine UMETA(DisplayName = "Magazine"),
	AT_Underbarrel UMETA(DisplayName = "Underbarrel"),
	AT_Stock UMETA(DisplayName = "Stock")
};

UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	CT_None UMETA(DisplayName = "None"),
	CT_Health UMETA(DisplayName = "Health"),
	CT_Stamina UMETA(DisplayName = "Stamina"),
	CT_Adrenaline UMETA(DisplayName = "Adrenaline"),
	CT_WhiteKarma UMETA(DisplayName = "White Karma"),
	CT_BlackKarma UMETA(DisplayName = "Black Karma")
};

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ST_Inventory UMETA(DisplayName = "Inventory"),
	ST_Attachment UMETA(DisplayName = "Attachment"),
	ST_QuickSlot UMETA(DisplayName = "Quick Slot"),
	ST_Weapon UMETA(DisplayName = "Weapon"),
	ST_Armor UMETA(DisplayName = "Armor")
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RequiredAmmoID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentType AttachmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConsumableType ConsumableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> AttachmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<ABaseItem> ItemClass;

	// Transform 조정용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform WeaponSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PowerAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttachmentType, FName> AttachmentState;

};

USTRUCT(BlueprintType)
struct FCraftingIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct FCraftingRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResultItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResultQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingIngredient> RequiredIngredients;
};