// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Crafting/CraftingIngredientSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Core/ItemDataSubsystem.h"

void UCraftingIngredientSlotWidget::InitIngredientSlot(FName ItemID, int32 OwnedQuantity, int32 RequiredQuantity)
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);

	if (Img_IngredientIcon)
	{
		UTexture2D* IconTexture = ItemData.Icon.LoadSynchronous();
		if (IconTexture)
		{
			Img_IngredientIcon->SetBrushFromTexture(IconTexture);
		}
	}

	if (Txt_Amount)
	{
		FString AmountText = FString::Printf(TEXT("%d / %d"), OwnedQuantity, RequiredQuantity);
		Txt_Amount->SetText(FText::FromString(AmountText));

		if (OwnedQuantity >= RequiredQuantity)
		{
			Txt_Amount->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			Txt_Amount->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
	}
}
