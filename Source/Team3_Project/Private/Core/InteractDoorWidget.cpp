// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InteractDoorWidget.h"
#include "Core/QuestTableWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"


void UInteractDoorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BT_OpenTable)
	{
		BT_OpenTable->OnClicked.AddDynamic(this, &UInteractDoorWidget::OnClickedOpen);
	}

	if (BT_Close)
	{
		BT_Close->OnClicked.AddDynamic(this, &UInteractDoorWidget::OnClickedClose);
	}
}

void UInteractDoorWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UInteractDoorWidget::OnClickedOpen()
{
	if (!QuestTableWidgetClass) return;


	if (!IsValid(QuestTableWidgetInstance))
	{
		QuestTableWidgetInstance = CreateWidget<UUserWidget>(this, QuestTableWidgetClass);
	}

	if (IsValid(QuestTableWidgetInstance) && (!QuestTableWidgetInstance->IsInViewport()))
	{
		QuestTableWidgetInstance->AddToViewport();
	}

	RemoveFromParent();
}

void UInteractDoorWidget::OnClickedClose()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC == nullptr) return;

	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;
	RemoveFromParent();
}


