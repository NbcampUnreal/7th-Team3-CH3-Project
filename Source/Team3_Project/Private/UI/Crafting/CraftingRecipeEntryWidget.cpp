// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Crafting/CraftingRecipeEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UCraftingRecipeEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnClicked.AddDynamic(this, &UCraftingRecipeEntryWidget::OnSelectButtonClicked);
	}
}

void UCraftingRecipeEntryWidget::InitRecipeWidgetEntry(FName InRecipeID, const FString& RecipeName, bool bCanCraft)
{
	CurrentRecipeID = InRecipeID;

	if (Txt_RecipeName)
	{
		Txt_RecipeName->SetText(FText::FromString(RecipeName));
	}

	if (Txt_Status)
	{
		if (bCanCraft)
		{
			Txt_Status->SetText(FText::FromString(TEXT("제작 가능")));
			Txt_Status->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			Txt_Status->SetText(FText::FromString(TEXT("제작 불가")));
			Txt_Status->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
	}
}

void UCraftingRecipeEntryWidget::OnSelectButtonClicked()
{
	if (OnRecipeSelected.IsBound())
	{
		OnRecipeSelected.Broadcast(CurrentRecipeID);
	}
}