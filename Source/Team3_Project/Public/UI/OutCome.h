#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OutCome.generated.h"


UCLASS()
class TEAM3_PROJECT_API UOutCome : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UOutCome(const FObjectInitializer& ObjectInitializer);

protected:
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_ResultTitle;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_FinalScore;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_FinalKill;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Restart;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Quit;

    float CurrentOpacity;
    float FadeSpeed; 
    bool bStartFade;

    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UFUNCTION(BlueprintCallable) void SetupOutcome(bool bIsVictory, int32 Score, int32 Kill);

    UFUNCTION() void OnRestartClicked();
    UFUNCTION() void OnQuitClicked();
};
