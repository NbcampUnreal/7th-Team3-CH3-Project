// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/ItemTypes.h"
#include "CraftingMainWidget.generated.h"

class UScrollBox;
class UHorizontalBox;
class UImage;
class UButton;
class UDataTable;
class UTextBlock;
class UCraftingRecipeEntryWidget;
class UCraftingIngredientSlotWidget;
class UCraftingConfirmWidget;
class UVerticalBox;

UCLASS()
class TEAM3_PROJECT_API UCraftingMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* RecipeListScroll;

	UPROPERTY(meta = (BindWidget))
	UImage* ResultItemIcon;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* IngredientsBox;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Craft;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_ResultNameAndQuantity;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Panel_RecipeDetails;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	UDataTable* CraftingRecipesTable;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Classes")
	TSubclassOf<UCraftingRecipeEntryWidget> RecipeEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Classes")
	TSubclassOf<UCraftingIngredientSlotWidget> IngredientSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Classes")
	TSubclassOf<UCraftingConfirmWidget> ConfirmWidgetClass;

	FName CurrentSelectedRecipeID;

	UFUNCTION()
	void InitializeRecipes();

	UFUNCTION()
	void UpdateRecipeDetails(FName RecipeID);

	UFUNCTION()
	void OnCraftButtonClicked();

	UFUNCTION()
	void HandleCraftConfirm(bool bIsConfirmed);
};
