#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Pause.generated.h"

UCLASS()
class TEAM3_PROJECT_API UPause : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) class UButton* Btn_Back;
	UPROPERTY(meta = (BindWidget)) class UButton* Btn_MainMenu;

	//virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION() void OnBackClicked();
	UFUNCTION() void OnMainMenuClicked();
};
