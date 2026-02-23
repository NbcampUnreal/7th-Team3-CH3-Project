// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryTooltip.h"
#include "Components/TextBlock.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UInventoryTooltip::UpdateTooltip(const FInventoryItem& Item)
{
	// 유효하지 않은 아이템이면 표시하지 않음
	if (Item.ItemID.IsNone())
	{
		return;
	}

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

	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(Item.ItemID);

	// 텍스트 위젯에 이름/설명 바인딩
	if (Txt_Name)
	{
		Txt_Name->SetText(ItemData.DisplayName);
	}
	if (Txt_Description)
	{
		Txt_Description->SetText(ItemData.Description);
	}
}
