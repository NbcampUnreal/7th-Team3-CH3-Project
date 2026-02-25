#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
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

    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_1;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_2;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_3;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_4;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_5;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_6;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_7;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_SlotNum_8;

    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_CurrentAmmo;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_MaxAmmo;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_Divider;

    UPROPERTY() TArray<class UTextBlock*> SlotNumArray;

    UPROPERTY() class UItemDataSubsystem* ItemDataSubsystem;
public:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_0;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_1;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_2;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_3;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_4;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_5;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_6;
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Slot_7;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly) class UImage* Img_GunInformation;

protected:
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
    TArray<UImage*> SlotArray;
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
    UFUNCTION(BlueprintCallable, Category = "HUD") void AddNewQuest(int32 ID, FString Title, FString Desc, int32 MaxCount);
    UFUNCTION(BlueprintCallable, Category = "HUD") void FinishQuest(int32 ID);
    UFUNCTION(BlueprintCallable, Category = "HUD") void ReceiveTeamData(bool bSuccess, int32 Bonus);

    UFUNCTION(BlueprintCallable, Category = "HUD") void RefreshQuestIcon();
    UFUNCTION(BlueprintCallable, Category = "HUD") void HideWaveMessage();

    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateQuickSlotImage(int32 SlotIndex, UTexture2D* IconTexture);
    UFUNCTION(BlueprintCallable, Category = "HUD") void UpdateQuickSlotUI();
    UFUNCTION() void OnQuickSlotItemChanged(int32 SlotIndex, FName ItemID);
    UFUNCTION() void OnEquipmentChanged();
    UFUNCTION() void OnQuickSlotRefreshAll();
    UFUNCTION(BlueprintCallable) void HighlightQuickSlot(int32 SlotIndex);
    UFUNCTION() void OnWeaponEquipChanged(bool bIsEquipping, FName ItemID);
    UFUNCTION(BlueprintCallable, Category = "HUD") void OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);
    UFUNCTION(BlueprintCallable) void UpdateQuestCount(int32 ID, FString Title, FString Desc, int32 Current, int32 Max);
};
