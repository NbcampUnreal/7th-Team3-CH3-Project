// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMainWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventoryItemSlot;
class UPanelWidget;
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

protected:
	void NativeConstruct() override;

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
	TSubclassOf<UUserWidget> ContextMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory UI")
	int32 Columns = 5;

private:
	bool bIsContextMenuOpen = false;

	UPROPERTY()
	UUserWidget* ActiveTooltip;

	UPROPERTY()

	UUserWidget* ActiveContextMenu;

	void ShowTooltip(const FInventoryItem& Item);
	void HideTooltip();
	void OpenContextMenu(const FInventoryItem& Item);
	void CloseContextMenu();
	void UpdateTooltipPosition();
};
