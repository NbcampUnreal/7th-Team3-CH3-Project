// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/NextLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UNextLevelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (BT_Open)
	{
		BT_Open->OnClicked.AddDynamic(this, &UNextLevelWidget::OnClickedOpen);
	}

	if (BT_Close)
	{
		BT_Close->OnClicked.AddDynamic(this, &UNextLevelWidget::OnClickedClose);
	}
}

void UNextLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UNextLevelWidget::OnClickedOpen()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC == nullptr) return;
	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;

	UGameplayStatics::OpenLevel(GetWorld(), OpenLevelName);
}

void UNextLevelWidget::OnClickedClose()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC == nullptr) return;

	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;
	RemoveFromParent();
}