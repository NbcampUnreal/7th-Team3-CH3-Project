// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NextLevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API UNextLevelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Level | Name")
	FName OpenLevelName;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_Open;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BT_Close;

	UFUNCTION()
	void OnClickedOpen();

	UFUNCTION()
	void OnClickedClose();
};
