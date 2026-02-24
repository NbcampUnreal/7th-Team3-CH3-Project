// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingConfirmWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmResult, bool, bIsConfirmed);

UCLASS()
class TEAM3_PROJECT_API UCraftingConfirmWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void SetupPopup(const FString& ItemName);

	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnConfirmResult OnConfirmResult;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Message;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Yes;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_No;

private:
	UFUNCTION()
	void OnYesBtnClicked();
	UFUNCTION()
	void OnNoBtnClicked();


};
