// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "UI/Inventory/InventoryItemSlot.h"
#include "InventoryActionMenu.generated.h"


class UButton;
class UInventoryComponent;
class USpacer;
class UInventoryMainWidget;

UCLASS()
class TEAM3_PROJECT_API UInventoryActionMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	void Setup(const FInventoryItem& Item, UInventoryComponent* InventoryComp, ESlotType SlotType, int32 SlotIndex, UInventoryMainWidget* ParentWidget);

protected:

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Use;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Drop;

	UPROPERTY(meta = (BindWidget))
	USpacer* ActionSpacer;

	ESlotType CurrentSlotType;
	int32 CurrentSlotIndex;

	UPROPERTY()
	UInventoryMainWidget* ParentInventoryWidget;

private:
	FInventoryItem TargetItem;

	UPROPERTY()
	UInventoryComponent* LinkedInventoryComp;

	UFUNCTION()
	void OnUseClicked();

	UFUNCTION()
	void OnDropClicked();
};
