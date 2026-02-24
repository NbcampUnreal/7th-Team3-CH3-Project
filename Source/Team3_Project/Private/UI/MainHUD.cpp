#include "UI/MainHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "UI/QuestItemWidget.h"
#include "UI/ResultWidget.h"
#include "Item/InventoryComponent.h"

UMainHUD::UMainHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CurrHealth = TargetHealth = 1.0f;
    CurrStamina = TargetStamina = 1.0f;
    CurrWhiteKarma = TargetWhiteKarma = 0.5f;
    CurrBlackKarma = TargetBlackKarma = 0.5f;
    PendingBonus = 100;
    bPendingSuccess = true;
}

void UMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // 델리게이트 바인딩
    OnWhiteKarmaChanged.AddDynamic(this, &UMainHUD::UpdateWhiteKarma);
    OnBlackKarmaChanged.AddDynamic(this, &UMainHUD::UpdateBlackKarma);
    OnScoreChanged.AddDynamic(this, &UMainHUD::UpdateScore);
    OnKillsChanged.AddDynamic(this, &UMainHUD::UpdateKills);
    OnHealthChanged.AddDynamic(this, &UMainHUD::UpdateHealth);
    OnStaminaChanged.AddDynamic(this, &UMainHUD::UpdateStamina);
    OnQuestAdded.AddDynamic(this, &UMainHUD::AddNewQuest);
    OnQuestFinished.AddDynamic(this, &UMainHUD::FinishQuest);
    OnWaveStarted.AddDynamic(this, &UMainHUD::StartWaveUI);
    OnWaveEnded.AddDynamic(this, &UMainHUD::ReceiveTeamData);



    if (Timer) Timer->SetVisibility(ESlateVisibility::Collapsed);
    RefreshQuestIcon();
}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // WhiteKarma
    CurrWhiteKarma = FMath::FInterpTo(CurrWhiteKarma, TargetWhiteKarma, InDeltaTime, InterpSpeed);
    if (PB_WhiteKarma) PB_WhiteKarma->SetPercent(CurrWhiteKarma);

    // BlackKarma
    CurrBlackKarma = FMath::FInterpTo(CurrBlackKarma, TargetBlackKarma, InDeltaTime, InterpSpeed);
    if (PB_BlackKarma) PB_BlackKarma->SetPercent(CurrBlackKarma);

    // Health
    CurrHealth = FMath::FInterpTo(CurrHealth, TargetHealth, InDeltaTime, InterpSpeed);
    if (PB_Health)
    {
        PB_Health->SetPercent(CurrHealth);

        FLinearColor TargetColor;
        if (CurrHealth > 0.5f)
        {
            // 0.5(주황) ~ 1.0(초록) 사이를 부드럽게 연결 
            float Alpha = (CurrHealth - 0.5f) * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor(1.0f, 0.5f, 0.0f), FLinearColor::Green, Alpha);
        }
        else
        {
            // 0.0(빨강) ~ 0.5(주황) 사이를 부드럽게 연결 
            float Alpha = CurrHealth * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor(1.0f, 0.5f, 0.0f), Alpha);
        }
        PB_Health->SetFillColorAndOpacity(TargetColor);
    }

    // Stamina
    CurrStamina = FMath::FInterpTo(CurrStamina, TargetStamina, InDeltaTime, InterpSpeed);
    if (PB_Stamina)
    {
        PB_Stamina->SetPercent(CurrStamina);

        FLinearColor TargetColor;
        if (CurrStamina > 0.5f)
        {
            float Alpha = (CurrStamina - 0.5f) * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor(1.0f, 0.5f, 0.0f), FLinearColor::Green, Alpha);
        }
        else
        {
            float Alpha = CurrStamina * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor(1.0f, 0.5f, 0.0f), Alpha);
        }
        PB_Stamina->SetFillColorAndOpacity(TargetColor);
    }

    // Timer (/ EndWaveUI)
    if (bIsTimerActive)
    {
        CurrentRemainingTime -= InDeltaTime;
        
        if (Timer)
        {
            int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(CurrentRemainingTime));
            int32 Min = TotalSeconds / 60;
            int32 Sec = TotalSeconds % 60;
            FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Min, Sec);
            Timer->SetText(FText::FromString(TimeStr));

            if (CurrentRemainingTime <= 10.0f)
            {
                Timer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            }
            else
            {
                Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
        }

        if (CurrentRemainingTime <= 0.0f)
        {
            bIsTimerActive = false;
            EndWaveUI(bPendingSuccess, PendingBonus);
        }
    }
}

void UMainHUD::AddNewQuest(int32 ID, FString Title, FString Desc)
{
    if (QuestList && QuestItemClass)
    {
        UQuestItemWidget* NewItem = CreateWidget<UQuestItemWidget>(this, QuestItemClass);
        if (NewItem)
        {
            NewItem->SetQuestText(ID, Title, Desc); // ID 전달
            QuestList->AddChildToVerticalBox(NewItem);
            QuestMap.Add(ID, NewItem);
            RefreshQuestIcon();
        }
    }
}

void UMainHUD::FinishQuest(int32 ID)
{
    if (QuestMap.Contains(ID))
    {
        QuestMap[ID]->CompleteQuest();
        QuestMap.Remove(ID); // 맵에서 즉시 제거하여 중복 방지 

        FTimerHandle IconHandle;
        GetWorld()->GetTimerManager().SetTimer(IconHandle, this, &UMainHUD::RefreshQuestIcon, 3.1f, false);
    }
}

void UMainHUD::UpdateScore(int32 Score)
{
    CurrentScore = Score;
    if (TotalScore)
    {
        TotalScore->SetText(FText::Format(FText::FromString("Score : {0}"), Score));
    }
}

void UMainHUD::UpdateKills(int32 Kills) 
{
    CurrentKills = Kills; 
    if (TotalKills)
    {
        TotalKills->SetText(FText::Format(FText::FromString("Kill : {0}"), Kills));
    }
}

void UMainHUD::UpdateHealth(float Health) 
{ 
    TargetHealth = Health; 
}

void UMainHUD::UpdateStamina(float Stamina) 
{
    TargetStamina = Stamina; 
}

void UMainHUD::UpdateWhiteKarma(float White) 
{ 
    TargetWhiteKarma = White; 
}

void UMainHUD::UpdateBlackKarma(float Black) 
{ 
    TargetBlackKarma = Black; 
}

void UMainHUD::StartWaveUI(FString Message, float WaveTime)
{
    TempWaveScore = CurrentScore;
    TempWaveKills = CurrentKills;
    CurrentRemainingTime = WaveTime;
    bIsTimerActive = true;

    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Visible);
    }
    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetText(FText::FromString(Message));
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Visible);

        FTimerHandle MsgTimerHandle;
 
        GetWorld()->GetTimerManager().SetTimer(
            MsgTimerHandle,
            this,
            &UMainHUD::HideWaveMessage,
            3.0f,
            false
        );
    }
}

void UMainHUD::EndWaveUI(bool bSuccess, int32 Bonus)
{
    bIsTimerActive = false;
    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Collapsed);
    }

    int32 FinalWaveScore = CurrentScore - TempWaveScore; 

    if (bSuccess)
    {
        CurrentScore += Bonus;
        UpdateScore(CurrentScore);
    }

    int32 BonusToSend = bSuccess ? Bonus : 0;

    if (ResultWidgetClass)
    {
        UResultWidget* ResultUI = CreateWidget<UResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultUI)
        {
            ResultUI->AddToViewport();
            ResultUI->SetupSuccess(CurrentKills - TempWaveKills, FinalWaveScore, Bonus, bSuccess);
        }
    }
}

void UMainHUD::RefreshQuestIcon() 
{ 
    if (img_Quest)
    {
        img_Quest->SetVisibility(QuestMap.Num() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UMainHUD::HideWaveMessage()
{
    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UMainHUD::ReceiveTeamData(bool bSuccess, int32 Bonus)
{
    bPendingSuccess = bSuccess;
    PendingBonus = Bonus;
}