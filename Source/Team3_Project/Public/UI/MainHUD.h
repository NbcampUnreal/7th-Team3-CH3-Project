#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDInt32Changed, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDFloatChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStarted, FString, Message, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveEnded, bool, bSuccess, int32, Bonus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestAdded, int32, QuestID, FString, Title, FString, Desc);

UCLASS()
class TEAM3_PROJECT_API UMainHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UMainHUD(const FObjectInitializer& ObjectInitializer);

    // --- 델리게이트 변수 --- 
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDFloatChanged OnWhiteKarmaChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDFloatChanged OnBlackKarmaChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDInt32Changed OnScoreChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDInt32Changed OnKillsChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDFloatChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDFloatChanged OnStaminaChanged;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnQuestAdded OnQuestAdded;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnHUDInt32Changed OnQuestFinished;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnWaveStarted OnWaveStarted;
    UPROPERTY(BlueprintAssignable, Category = "HUD|Events") FOnWaveEnded OnWaveEnded;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // 위젯 바인딩 
    UPROPERTY(meta = (BindWidget)) class UTextBlock* TotalScore;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* TotalKills;
    UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Health;
    UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Stamina;
    UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_WhiteKarma;
    UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_BlackKarma;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Timer;
    UPROPERTY(meta = (BindWidget)) class UVerticalBox* QuestList;
    UPROPERTY(meta = (BindWidget)) class UImage* img_Quest;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_WaveMessage;

    UPROPERTY(EditAnywhere, Category = "Quest") TSubclassOf<class UQuestItemWidget> QuestItemClass;
    UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<class UResultWidget> ResultWidgetClass;

    // 내부 상태 및 보간 변수 
    float InterpSpeed = 5.0f;
    float CurrWhiteKarma, TargetWhiteKarma;
    float CurrBlackKarma, TargetBlackKarma;
    float CurrHealth, TargetHealth;
    float CurrStamina, TargetStamina;
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentKills; 
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentScore;
    int32 TempWaveKills, TempWaveScore;
    int32 PendingBonus;
    float CurrentRemainingTime;
    bool bIsTimerActive;
    bool bPendingSuccess;

    UPROPERTY() TMap<int32, class UQuestItemWidget*> QuestMap;

public:
    // --- 외부 호출 가능 함수 --- 
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateScore(int32 Score);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateKills(int32 Kills);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateHealth(float Health);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateStamina(float Stamina);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateWhiteKarma(float White);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateBlackKarma(float Black);
    UFUNCTION(BlueprintCallable, Category = "HUD") void StartWaveUI(FString Message, float WaveTime);
    UFUNCTION(BlueprintCallable, Category = "HUD") void EndWaveUI(bool bSuccess, int32 Bonus);
    UFUNCTION(BlueprintCallable, Category = "HUD") void AddNewQuest(int32 ID, FString Title, FString Desc);
    UFUNCTION(BlueprintCallable, Category = "HUD") void FinishQuest(int32 ID);
    UFUNCTION(BlueprintCallable, Category = "HUD") void ReceiveTeamData(bool bSuccess, int32 Bonus);
    UFUNCTION(BlueprintCallable, Category = "HUD") void RefreshQuestIcon();
    UFUNCTION(BlueprintCallable, Category = "HUD") void HideWaveMessage();
};
