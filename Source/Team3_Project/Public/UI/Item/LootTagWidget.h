// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "LootTagWidget.generated.h"

class UTextBlock;
class UImage;
class UHorizontalBox;
class USizeBox;

UCLASS()
class TEAM3_PROJECT_API ULootTagWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LootTag")
	void InitLootTag(FName InItemID, int32 InQuantity);

	UFUNCTION(BlueprintCallable, Category = "LootTag")
	void UpdateAttachmentIcons(const TMap<EAttachmentType, FName>& Attachments);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Quantity;

	UPROPERTY(meta = (BindWidgetOptional))
	UHorizontalBox* HB_Attachments;
	
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LTImg_Scope;
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LTImg_Barrel;
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LTImg_Magazine;
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LTImg_Underbarrel;
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LTImg_Stock;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_Scope;
	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_Barrel;
	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_Magazine;
	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_Underbarrel;
	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_Stock;

};
