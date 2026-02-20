// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LootTagWidget.generated.h"

class UTextBlock;

UCLASS()
class TEAM3_PROJECT_API ULootTagWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LootTag")
	void InitLootTag(FName InItemID, int32 InQuantity);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Quantity;
	
};
