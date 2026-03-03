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
	
public:
	UStartMenu(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(meta = (BindWidget)) UButton* Btn_Start;

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION() void OnStartClicked();
	//UFUNCTION() void MoveToGameLevel();

	float CurrentAlpha;
	float FadeDir;   
	bool bIsTransitioning;
};
