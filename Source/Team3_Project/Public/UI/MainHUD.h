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

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* QuestList;

    UPROPERTY(meta = (BindWidget))
    class UImage* img_Quest;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_WaveMessage;

    UPROPERTY(EditAnywhere, Category = "Quest")
    TSubclassOf<class UQuestItemWidget> QuestItemClass;
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UResultWidget> ResultWidgetClass;

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

    UPROPERTY(BlueprintReadWrite, Category = "HUD")
    int32 CurrentKills;
    UPROPERTY(BlueprintReadWrite, Category = "HUD")
    int32 CurrentScore;

    UPROPERTY()
    int32 CurrentQuestCount;

    int32 TempWaveKills;
    int32 TempWaveScore;

    float CurrentRemainingTime;
    bool bIsTimerActive;
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

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void FinishQuest(FString Title);

    void UpdateDynamicColor(class UProgressBar* Bar, float CurrentPercent);
    void AddNewQuest(FString Title, FString Desc);

    UFUNCTION(BlueprintCallable)
    void ShowWaveResult(bool bSuccess, int32 Bonus, int32 InScore, int32 InKills);

    void SetQuestIconVisibility(bool bVisible);
    void RefreshQuestIcon();
    void RemoveQuest();
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void StartWaveUI(FString Message, float WaveTime);
    UFUNCTION(BlueprintCallable, Category = "HUD Test")
    void EndWaveUI(bool bSuccess, int32 Score, int32 Bonus, int32 Kills);
    void HideWaveMessage();
};
