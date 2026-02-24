// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingIngredientSlotWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class TEAM3_PROJECT_API UCraftingIngredientSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void InitIngredientSlot(FName ItemID, int32 OwnedQuantity, int32 RequiredQuantity);

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* Img_IngredientIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Amount;
};
