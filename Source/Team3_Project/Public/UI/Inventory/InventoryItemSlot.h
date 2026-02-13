// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "Input/Reply.h"
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

	void SetSelectedSlot(bool bSelected);

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantity;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;

	FName CurrentItemID;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* SelectionBorder;

	FVector2D DragOffset;
};
