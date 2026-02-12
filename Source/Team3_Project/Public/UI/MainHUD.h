#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MainHUD.generated.h"

UCLASS()
class TEAM3_PROJECT_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
    UMainHUD(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalScore;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalKills;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* PB_Health;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* PB_Stamina;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* PB_WhiteKarma;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* PB_BlackKarma;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Timer;

    UPROPERTY(EditAnywhere, Category = "UI Style")
    float InterpSpeed;

    float CurrHealth;
    float TargetHealth;

    float CurrStamina;
    float TargetStamina;

    float CurrWhiteKarma;
    float TargetWhiteKarma;
    float CurrBlackKarma;
    float TargetBlackKarma;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateScore(int32 Score);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateKills(int32 Kills);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateHealth(float Health);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateStamina(float Stamina);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateWhiteKarma(float WhiteKarma);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateBlackKarma(float BlackKarma);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void UpdateTime(float Time);

    void UpdateDynamicColor(class UProgressBar* Bar, float CurrentPercent);
};
