// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "InventoryItemSlot.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class TEAM3_PROJECT_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void InitSlot(const FInventoryItem& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void ClearSlot();

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantity;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;

	FName CurrentItemID;
};
