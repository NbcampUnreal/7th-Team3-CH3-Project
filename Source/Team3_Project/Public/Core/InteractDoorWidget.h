// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractDoorWidget.generated.h"



/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API UInteractDoorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_OpenTable;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_Close;

	UPROPERTY(EditDefaultsOnly, Category = "Class | OpenUI")
	TSubclassOf<UUserWidget> QuestTableWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> QuestTableWidgetInstance;
	

	UFUNCTION()
	void OnClickedOpen();

	UFUNCTION()
	void OnClickedClose();
};
