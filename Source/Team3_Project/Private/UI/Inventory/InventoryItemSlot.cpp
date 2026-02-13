// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryItemSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "UI/Inventory/InventoryDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventoryMainWidget.h"

void UInventoryItemSlot::InitSlot(const FInventoryItem& Item)
{
	//ItemID를 저장
	CurrentItemID = Item.ItemID;

	//수량 설정
	if (ItemQuantity)
	{
		if (Item.Quantity > 1)
		{
			ItemQuantity->SetText(FText::AsNumber(Item.Quantity));
			ItemQuantity->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//아이콘 설정
	if (ItemIcon)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		if (GameInstance)
		{
			UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
			if (ItemDataSubsystem)
			{
				FItemData ItemData = ItemDataSubsystem->GetItemDataByID(Item.ItemID);
				UTexture2D* IconTexture = ItemData.Icon.LoadSynchronous();
				if (IconTexture)
				{
					ItemIcon->SetBrushFromTexture(IconTexture);
					ItemIcon->SetVisibility(ESlateVisibility::Visible);
					if (SlotButton)
					{
						SlotButton->SetIsEnabled(true);
					}
				}
			}
		}
	}
}

void UInventoryItemSlot::ClearSlot()
{
	CurrentItemID = NAME_None;
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	if (ItemQuantity)
	{
		ItemQuantity->SetText(FText::GetEmpty());
		ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SlotButton)
	{
		SlotButton->SetIsEnabled(false);
	}
}

void UInventoryItemSlot::SetSelectedSlot(bool bSelected)
{
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UInventoryItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (!CurrentItemID.IsNone() && ParentInventoryWidget)
	{
		FInventoryItem DummyItem;
		DummyItem.ItemID = CurrentItemID;
		ParentInventoryWidget->HandleSlotHover(this, DummyItem);
	}
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->HandleSlotUnhover(this);
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (!CurrentItemID.IsNone() && ParentInventoryWidget)
		{
			FInventoryItem DummyItem;
			DummyItem.ItemID = CurrentItemID;
			ParentInventoryWidget->HandleSlotRightClick(this, DummyItem);
			return FReply::Handled();
		}
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		SetSelectedSlot(true);

		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (CurrentItemID.IsNone())
	{
		return;
	}

	UInventoryDragDropOperation* DragDropOp = NewObject<UInventoryDragDropOperation>();
	DragDropOp->ItemID = CurrentItemID;
	DragDropOp->Pivot = EDragPivot::CenterCenter;

	UInventoryItemSlot* DragVisual = CreateWidget<UInventoryItemSlot>(this, this->GetClass());
	if (DragVisual)
	{
		FInventoryItem DragItem;
		DragItem.ItemID = CurrentItemID;
		DragItem.Quantity = 1; //드래그 시 1개만 드래그한다고 가정
		DragVisual->InitSlot(DragItem);

		DragDropOp->DefaultDragVisual = DragVisual;
	}

	OutOperation = DragDropOp;
}
