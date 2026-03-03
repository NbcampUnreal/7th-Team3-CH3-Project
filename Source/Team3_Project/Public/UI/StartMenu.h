#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "StartMenu.generated.h"

class UButton;

UCLASS()
class TEAM3_PROJECT_API UStartMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) UButton* Btn_Start;

	virtual void NativeOnInitialized() override;

	UFUNCTION() void OnStartClicked();
};
