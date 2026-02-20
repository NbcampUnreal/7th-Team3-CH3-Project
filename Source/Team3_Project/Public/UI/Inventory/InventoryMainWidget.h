// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "InventoryMainWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventoryItemSlot;
class UPanelWidget;
class UImage;
class UInventoryActionMenu;
struct FInventoryItem;

UCLASS()
class TEAM3_PROJECT_API UInventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void InitializeInventory(UInventoryComponent* NewInventoryComp);

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void RefreshInventory();

	void HandleSlotHover(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item);
	void HandleSlotUnhover(UInventoryItemSlot* ItemSlot);
	void HandleSlotRightClick(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item);

	bool IsContextMenuOpen() const { return bIsContextMenuOpen; }

	void UpdateWeaponPanel();

	void HandleSlotSelection(UInventoryItemSlot* NewSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }

	void OpenContextMenu(const FInventoryItem& Item, ESlotType SlotType, int32 SlotIndex);
	void CloseContextMenu();

protected:
	void NativeConstruct() override;

	void NativeDestruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* QuickSlotPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
	TSubclassOf<UInventoryItemSlot> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
	TSubclassOf<UUserWidget> TooltipWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
	TSubclassOf<UInventoryActionMenu> ContextMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory UI")
	int32 Columns = 5;

	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Scope;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Barrel;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Magazine;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Underbarrel;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Stock;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Weapon;
	UPROPERTY(meta = (BindWidgetOptional))
	UInventoryItemSlot* Slot_Armor;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* Img_Weapon;

	UPROPERTY()
	UInventoryItemSlot* LastSelectedSlot;

private:
	bool bIsContextMenuOpen = false;

	UPROPERTY()
	UUserWidget* ActiveTooltip;

	UPROPERTY()

	UInventoryActionMenu* ActiveContextMenu;

	void ShowTooltip(const FInventoryItem& Item);
	void HideTooltip();
	void UpdateTooltipPosition();
	void UpdateAttachmentSlot(UInventoryItemSlot* Slot, EAttachmentType Type, const TMap<EAttachmentType, FName>& Attachments);
};
