// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Item/LootTagWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
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
		if (ItemData.ItemType == EItemType::IT_Weapon ||
			ItemData.ItemType == EItemType::IT_Armor ||
			ItemData.ItemType == EItemType::IT_Attachment)
		{
			Txt_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Txt_Quantity->SetVisibility(ESlateVisibility::Visible);
			Txt_Quantity->SetText(FText::Format(NSLOCTEXT("LootTag", "Qty", "x{0}"), InQuantity));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitLootTag: Txt_Quantity widget is null"));
	}

	if (HB_Attachments)
	{
		if (ItemData.ItemType == EItemType::IT_Weapon)
		{
			HB_Attachments->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("InitLootTag: ItemID %s is a weapon, showing attachment icons"), *InItemID.ToString());
		}
		else
		{
			HB_Attachments->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Warning, TEXT("InitLootTag: ItemID %s is not a weapon, hiding attachment icons"), *InItemID.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitLootTag: HB_Attachments widget is null"));
	}
}

void ULootTagWidget::UpdateAttachmentIcons(const TMap<EAttachmentType, FName>& Attachments)
{
	if (LTImg_Scope)
	{
		LTImg_Scope->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LTImg_Barrel)
	{
		LTImg_Barrel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LTImg_Magazine)
	{
		LTImg_Magazine->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LTImg_Underbarrel)
	{
		LTImg_Underbarrel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LTImg_Stock)
	{
		LTImg_Stock->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SB_Scope)
	{
		SB_Scope->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SB_Barrel)
	{
		SB_Barrel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SB_Magazine)
	{
		SB_Magazine->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SB_Underbarrel)
	{
		SB_Underbarrel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SB_Stock)
	{
		SB_Stock->SetVisibility(ESlateVisibility::Collapsed);
	}

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

	for (const auto& pair : Attachments)
	{
		EAttachmentType Type = pair.Key;
		FName AttachmentID = pair.Value;

		if (AttachmentID.IsNone())
		{
			continue;
		}

		FItemData AttachmentData = ItemDataSubsystem->GetItemDataByID(AttachmentID);
		UTexture2D* IconTexture = AttachmentData.Icon.LoadSynchronous();

		switch (Type)
		{
		case EAttachmentType::AT_Scope:
			if (SB_Scope && LTImg_Scope)
			{
				LTImg_Scope->SetBrushFromTexture(IconTexture);
				LTImg_Scope->SetVisibility(ESlateVisibility::Visible);
				SB_Scope->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: Setting scope icon for attachment %s"), *AttachmentID.ToString());
			}
			break;
		case EAttachmentType::AT_Barrel:
			if (SB_Barrel && LTImg_Barrel)
			{
				LTImg_Barrel->SetBrushFromTexture(IconTexture);
				LTImg_Barrel->SetVisibility(ESlateVisibility::Visible);
				SB_Barrel->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: Setting barrel icon for attachment %s"), *AttachmentID.ToString());
			}
			break;
		case EAttachmentType::AT_Magazine:
			if (SB_Magazine && LTImg_Magazine)
			{
				LTImg_Magazine->SetBrushFromTexture(IconTexture);
				LTImg_Magazine->SetVisibility(ESlateVisibility::Visible);
				SB_Magazine->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: Setting magazine icon for attachment %s"), *AttachmentID.ToString());	
			}
			break;
		case EAttachmentType::AT_Underbarrel:
			if (SB_Underbarrel && LTImg_Underbarrel)
			{
				LTImg_Underbarrel->SetBrushFromTexture(IconTexture);
				LTImg_Underbarrel->SetVisibility(ESlateVisibility::Visible);
				SB_Underbarrel->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: Setting underbarrel icon for attachment %s"), *AttachmentID.ToString());
			}
			break;
		case EAttachmentType::AT_Stock:
			if (SB_Stock && LTImg_Stock)
			{
				LTImg_Stock->SetBrushFromTexture(IconTexture);
				LTImg_Stock->SetVisibility(ESlateVisibility::Visible);
				SB_Stock->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: Setting stock icon for attachment %s"), *AttachmentID.ToString());
			}
			break;
		default:
			break;
		}
	}

	if (Attachments.Num() <= 0)
	{
		if (HB_Attachments)
		{
			HB_Attachments->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Warning, TEXT("UpdateAttachmentIcons: No attachments, hiding attachment icons"));
		}
	}
}
