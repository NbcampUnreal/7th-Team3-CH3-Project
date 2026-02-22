// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "InventoryTooltip.generated.h"

class UTextBlock;

UCLASS()
class TEAM3_PROJECT_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:

	void UpdateTooltip(const FInventoryItem& Item);

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Name;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Description;
	
};
