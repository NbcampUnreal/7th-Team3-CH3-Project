// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

class ABaseItem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Weapon UMETA(DisplayName = "Weapon"),
	IT_Consumable UMETA(DisplayName = "Consumable"),
	IT_Armor UMETA(DisplayName = "Armor"),
	IT_Ammo UMETA(DisplayName = "Ammo"),
	IT_Attachment UMETA(DisplayName = "Attachment"),
	IT_Ingredient UMETA(DisplayName = "Ingredient"),
	IT_QuestItem UMETA(DisplayName = "Quest Item"),
	IT_None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Pistol UMETA(DisplayName = "Pistol"),
	WT_Rifle UMETA(DisplayName = "Rifle"),
	WT_Shotgun UMETA(DisplayName = "Shotgun"),
	WT_Sniper UMETA(DisplayName = "Sniper"),
	WT_Melee UMETA(DisplayName = "Melee"),
	WT_None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	AT_Scope UMETA(DisplayName = "Scope"),
	AT_Barrel UMETA(DisplayName = "Barrel"),
	AT_Magazine UMETA(DisplayName = "Magazine"),
	AT_Underbarrel UMETA(DisplayName = "Underbarrel"),
	AT_Stock UMETA(DisplayName = "Stock"),
	AT_None UMETA(DisplayName = "None")
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
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> AttachmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<ABaseItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PowerAmount;

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
struct FCraftingRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResultItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResultAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> RequiredIngredients;
};