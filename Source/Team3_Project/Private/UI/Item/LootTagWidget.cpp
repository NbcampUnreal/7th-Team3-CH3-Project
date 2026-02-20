// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Item/LootTagWidget.h"
#include "Components/TextBlock.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULootTagWidget::InitLootTag(FName InItemID, int32 InQuantity)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(InItemID);

	if (Txt_ItemName)
	{
		Txt_ItemName->SetText(ItemData.DisplayName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitLootTag: Txt_ItemName widget is null"));
	}

	if (Txt_Quantity)
	{
		if (InQuantity > 1)
		{
			Txt_Quantity->SetText(FText::Format(NSLOCTEXT("LootTag", "Qty", "x{0}"), InQuantity));
			Txt_Quantity->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Txt_Quantity->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitLootTag: Txt_Quantity widget is null"));
	}
}