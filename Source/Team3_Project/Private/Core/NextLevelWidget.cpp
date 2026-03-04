// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/NextLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Core/MainGameInstance.h"
#include "Item/InventoryComponent.h"
#include "Player/PlayerCharacter.h"
#include "Item/WeaponItem.h"

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
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not found."));
		return;
	}
	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance not found."));
		return;
	}
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found."));
		return;
	}
	UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent not found."));
		return;
	}


	GameInstance->SavedInventoryContents = InventoryComponent->GetInventoryContents();
	GameInstance->SavedQuickSlots = InventoryComponent->QuickSlots;

	if (PlayerCharacter->CurrentWeapon)
	{
		FInventoryItem WeaponData;
		WeaponData.ItemID = PlayerCharacter->CurrentWeapon->GetItemID();
		WeaponData.Quantity = 1;
		WeaponData.AttachmentState = PlayerCharacter->CurrentWeapon->GetAttachmentState();
		WeaponData.CurrentAmmo = PlayerCharacter->CurrentWeapon->GetCurrentAmmo();
		GameInstance->SavedWeapon = WeaponData;
	}
	else
	{
		GameInstance->SavedWeapon = FInventoryItem();
	}

	if (PlayerCharacter->bIsEquippedArmor)
	{
		FInventoryItem ArmorData;
		ArmorData.ItemID = PlayerCharacter->InventoryComponent->GetEquippedArmorID();
		ArmorData.Quantity = 1;
		GameInstance->SavedArmor = ArmorData;
	}
	else
	{
		GameInstance->SavedArmor = FInventoryItem();
	}

	GameInstance->bHasSavedData = true;

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