// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Crafting/CraftingMainWidget.h"
#include "UI/Crafting/CraftingRecipeEntryWidget.h"
#include "UI/Crafting/CraftingIngredientSlotWidget.h"
#include "UI/Crafting/CraftingConfirmWidget.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Kismet/GameplayStatics.h"

void UCraftingMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Craft)
	{
		Btn_Craft->OnClicked.AddDynamic(this, &UCraftingMainWidget::OnCraftButtonClicked);
		Btn_Craft->SetIsEnabled(false);
	}
	if (Panel_RecipeDetails)
	{
		Panel_RecipeDetails->SetVisibility(ESlateVisibility::Hidden);
	}
	InitializeRecipes();
}

void UCraftingMainWidget::InitializeRecipes()
{
	if (!CraftingRecipesTable || !RecipeListScroll || !RecipeEntryWidgetClass)
	{
		return;
	}

	RecipeListScroll->ClearChildren();

	TArray<FCraftingRecipe*> AllRecipes;
	CraftingRecipesTable->GetAllRows<FCraftingRecipe>(TEXT("CraftingContext"), AllRecipes);

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn)
	{
		return;
	}
	UInventoryComponent* InventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComp)
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

	for (FCraftingRecipe* Recipe : AllRecipes)
	{
		if (!Recipe)
		{
			continue;
		}

		UCraftingRecipeEntryWidget* RecipeEntry = CreateWidget<UCraftingRecipeEntryWidget>(this, RecipeEntryWidgetClass);
		if (RecipeEntry)
		{
			bool bCanCraft = true;
			for (const FCraftingIngredient& Ingredient : Recipe->RequiredIngredients)
			{
				if (!InventoryComp->HasItem(Ingredient.ItemID, Ingredient.Quantity))
				{
					bCanCraft = false;
					break;
				}
			}

			FItemData ResultItemData = ItemDataSubsystem->GetItemDataByID(Recipe->ResultItemID);
			FString RecipeName = ResultItemData.DisplayName.ToString();

			RecipeEntry->InitRecipeWidgetEntry(Recipe->ResultItemID, RecipeName, bCanCraft);
			RecipeEntry->OnRecipeSelected.AddDynamic(this, &UCraftingMainWidget::UpdateRecipeDetails);
			RecipeListScroll->AddChild(RecipeEntry);
		}
	}
}

void UCraftingMainWidget::UpdateRecipeDetails(FName RecipeID)
{
	CurrentSelectedRecipeID = RecipeID;

	if (!CraftingRecipesTable || !IngredientsBox || !IngredientSlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingRecipesTable, IngredientsBox, or IngredientSlotWidgetClass is not set in CraftingMainWidget"));
		return;
	}

	IngredientsBox->ClearChildren();

	TArray<FCraftingRecipe*> AllRecipes;
	CraftingRecipesTable->GetAllRows<FCraftingRecipe>(TEXT("CraftingContext"), AllRecipes);

	FCraftingRecipe* SelectedRecipe = nullptr;
	for (FCraftingRecipe* Recipe : AllRecipes)
	{
		if (Recipe && Recipe->ResultItemID == RecipeID)
		{
			SelectedRecipe = Recipe;
			break;
		}
	}

	if (!SelectedRecipe)
	{
		UE_LOG(LogTemp, Warning, TEXT("No crafting recipe found for RecipeID %s"), *RecipeID.ToString());
		return;
	}

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn)
	{
		return;
	}
	UInventoryComponent* InventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComp)
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

	FItemData ResultItemData = ItemDataSubsystem->GetItemDataByID(SelectedRecipe->ResultItemID);
	if (ResultItemIcon)
	{
		UTexture2D* IconTexture = ResultItemData.Icon.LoadSynchronous();
		if (IconTexture)
		{
			ResultItemIcon->SetBrushFromTexture(IconTexture);
		}
	}
	if (Txt_ResultNameAndQuantity)
	{
		FText FormattedText = FText::Format(FText::FromString(TEXT("{0} x {1}")),
			ResultItemData.DisplayName, SelectedRecipe->ResultQuantity);
		Txt_ResultNameAndQuantity->SetText(FormattedText);
	}

	bool bCanCraft = true;
	for (const FCraftingIngredient& Ingredient : SelectedRecipe->RequiredIngredients)
	{
		UCraftingIngredientSlotWidget* SlotWidget = CreateWidget<UCraftingIngredientSlotWidget>(this, IngredientSlotWidgetClass);
		if (SlotWidget)
		{
			int32 OwnedQuantity = InventoryComp->GetItemQuantity(Ingredient.ItemID);
			SlotWidget->InitIngredientSlot(Ingredient.ItemID, OwnedQuantity, Ingredient.Quantity);
			UHorizontalBoxSlot* BoxSlot = Cast<UHorizontalBoxSlot>(IngredientsBox->AddChild(SlotWidget));
			if (BoxSlot)
			{
				BoxSlot->SetPadding(FMargin(10.0f, 0.0f, 10.0f, 0.0f));
				BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			}

			if (OwnedQuantity < Ingredient.Quantity)
			{
				bCanCraft = false;
			}
		}
	}

	if (Btn_Craft)
	{
		Btn_Craft->SetIsEnabled(bCanCraft);
	}
	if (Panel_RecipeDetails)
	{
		Panel_RecipeDetails->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCraftingMainWidget::OnCraftButtonClicked()
{
	if (ConfirmWidgetClass && !CurrentSelectedRecipeID.IsNone())
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

		FItemData ResultItemData = ItemDataSubsystem->GetItemDataByID(CurrentSelectedRecipeID);

		UCraftingConfirmWidget* ConfirmWidget = CreateWidget<UCraftingConfirmWidget>(this, ConfirmWidgetClass);
		if (ConfirmWidget)
		{
			ConfirmWidget->SetupPopup(ResultItemData.DisplayName.ToString());
			ConfirmWidget->OnConfirmResult.AddDynamic(this, &UCraftingMainWidget::HandleCraftConfirm);
			ConfirmWidget->AddToViewport(100);
		}
	}
}

void UCraftingMainWidget::HandleCraftConfirm(bool bIsConfirmed)
{
	if (!bIsConfirmed || CurrentSelectedRecipeID.IsNone() || !CraftingRecipesTable)
	{
		return;
	}

	if (bIsConfirmed)
	{
		APawn* PlayerPawn = GetOwningPlayerPawn();
		if (!PlayerPawn)
		{
			return;
		}
		UInventoryComponent* InventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
		if (!InventoryComp)
		{
			return;
		}

		TArray<FCraftingRecipe*> AllRecipes;
		CraftingRecipesTable->GetAllRows<FCraftingRecipe>(TEXT("CraftingContext"), AllRecipes);
		FCraftingRecipe* SelectedRecipe = nullptr;

		for (FCraftingRecipe* Recipe : AllRecipes)
		{
			if (Recipe && Recipe->ResultItemID == CurrentSelectedRecipeID)
			{
				SelectedRecipe = Recipe;
				break;
			}
		}

		if (SelectedRecipe)
		{
			bool bCraftSuccess = InventoryComp->CraftItem(*SelectedRecipe);
			if (!bCraftSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Crafting failed for RecipeID %s"), *CurrentSelectedRecipeID.ToString());
			}
			else
			{
				InitializeRecipes();
				UpdateRecipeDetails(CurrentSelectedRecipeID);
				UE_LOG(LogTemp, Warning, TEXT("Crafting succeeded for RecipeID %s"), *CurrentSelectedRecipeID.ToString());
			}
		}
	}
}
