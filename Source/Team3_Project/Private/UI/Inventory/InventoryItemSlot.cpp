// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryItemSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

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
