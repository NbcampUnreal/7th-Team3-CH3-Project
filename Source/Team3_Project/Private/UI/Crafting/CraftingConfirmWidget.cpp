// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Crafting/CraftingConfirmWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UCraftingConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Yes)
	{
		Btn_Yes->OnClicked.AddDynamic(this, &UCraftingConfirmWidget::OnYesBtnClicked);
	}
	if (Btn_No)
	{
		Btn_No->OnClicked.AddDynamic(this, &UCraftingConfirmWidget::OnNoBtnClicked);
	}
}

void UCraftingConfirmWidget::SetupPopup(const FString& ItemName)
{
	if (Txt_Message)
	{
		FString Message = FString::Printf(TEXT("[%s]을(를) 제작하시겠습니까?"), *ItemName);
		Txt_Message->SetText(FText::FromString(Message));
	}
}



void UCraftingConfirmWidget::OnYesBtnClicked()
{
	if (OnConfirmResult.IsBound())
	{
		OnConfirmResult.Broadcast(true);
	}
	RemoveFromParent();
}

void UCraftingConfirmWidget::OnNoBtnClicked()
{
	if (OnConfirmResult.IsBound())
	{
		OnConfirmResult.Broadcast(false);
	}
	RemoveFromParent();
}
