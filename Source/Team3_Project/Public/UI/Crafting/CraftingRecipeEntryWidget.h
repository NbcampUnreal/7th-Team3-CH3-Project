// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingRecipeEntryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeSelected, FName, SelectedRecipeID);

class UButton;
class UTextBlock;

UCLASS()
class TEAM3_PROJECT_API UCraftingRecipeEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void InitRecipeWidgetEntry(FName InRecipeID, const FString& RecipeName, bool bCanCraft);

	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnRecipeSelected OnRecipeSelected;

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Select;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_RecipeName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Status;

private:
	FName CurrentRecipeID;

	UFUNCTION()
	void OnSelectButtonClicked();
	
};
