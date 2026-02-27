// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestTableWidget.generated.h"

class UInventoryComponent;

/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API UQuestTableWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_Accept;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_Close;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	UFUNCTION()
	void OnClickedAccept();

	UFUNCTION()
	void OnClickedClose();

	UFUNCTION()
	void UpdateButton(bool bHasQuestItem);

};
