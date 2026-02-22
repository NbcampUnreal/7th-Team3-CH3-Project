#include "UI/MainHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "UI/QuestItemWidget.h"
#include "Components/VerticalBox.h"
#include "UI/ResultWidget.h"

UMainHUD::UMainHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    InterpSpeed = 5.0f;

    CurrHealth = 1.0f;
    TargetHealth = 1.0f;

    CurrStamina = 1.0f;
    TargetStamina = 1.0f;

    CurrWhiteKarma = 0.5f;
    TargetWhiteKarma = 0.5f;
    CurrBlackKarma = 0.5f;
    TargetBlackKarma = 0.5f;

    CurrentQuestCount = 0;

    TempWaveKills = 0;
    TempWaveScore = 0;

    CurrentRemainingTime = 0.0f;
    bIsTimerActive = false;

    CurrentKills = 0;
    CurrentScore = 0;
}

void UMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Collapsed);
    }

    RefreshQuestIcon();

    /*AddNewQuest(TEXT("기본 목표"), TEXT("조작법을 익히고 전장으로 이동하세요."));
    AddNewQuest(TEXT("적 처치"), TEXT("주변의 적 5명을 처치하세요 (0/5)"));
    AddNewQuest(TEXT("생존"), TEXT("3분 동안 적의 공격으로부터 살아남으세요."));*/

}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (PB_Health)
    {
        CurrHealth = FMath::FInterpTo(CurrHealth, TargetHealth, InDeltaTime, InterpSpeed);
        PB_Health->SetPercent(CurrHealth);
        UpdateDynamicColor(PB_Health, CurrHealth);
    }

    if (PB_Stamina)
    {
        CurrStamina = FMath::FInterpTo(CurrStamina, TargetStamina, InDeltaTime, InterpSpeed);
        PB_Stamina->SetPercent(CurrStamina);
        UpdateDynamicColor(PB_Stamina, CurrStamina); 
    }

    if (PB_WhiteKarma)
    {
        CurrWhiteKarma = FMath::FInterpTo(CurrWhiteKarma, TargetWhiteKarma, InDeltaTime, InterpSpeed);
        PB_WhiteKarma->SetPercent(CurrWhiteKarma);
    }
    if (PB_BlackKarma)
    {
        CurrBlackKarma = FMath::FInterpTo(CurrBlackKarma, TargetBlackKarma, InDeltaTime, InterpSpeed);
        PB_BlackKarma->SetPercent(CurrBlackKarma);
    }

    if (bIsTimerActive)
    {
        CurrentRemainingTime -= InDeltaTime;

        if (CurrentRemainingTime <= 0.0f)
        {
            CurrentRemainingTime = 0.0f;
            bIsTimerActive = false; // 여기서 스위치를 꺼야 결과창이 한 번만 뜹니다.

            int32 PureWaveScore = CurrentScore - TempWaveScore;
            int32 PureWaveKills = CurrentKills - TempWaveKills;

            if (Timer) Timer->SetVisibility(ESlateVisibility::Collapsed);

            EndWaveUI(true, PureWaveScore, 1000, PureWaveKills);
        }
        else
        {
            UpdateTime(CurrentRemainingTime);
        }
    }
}

void UMainHUD::UpdateDynamicColor(UProgressBar* Bar, float CurrentPercent)
{
    if (!Bar) return;

    FLinearColor TargetColor;
    if (CurrentPercent <= 0.2f)
    {
        TargetColor = FLinearColor::Red;
    }
    else if (CurrentPercent <= 0.6f)
    {
        float Alpha = (CurrentPercent - 0.2f) / 0.4f;
        TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Yellow, Alpha);
    }
    else
    {
        float Alpha = (CurrentPercent - 0.6f) / 0.4f;
        TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, Alpha);
    }
    Bar->SetFillColorAndOpacity(TargetColor);
}

void UMainHUD::UpdateScore(int32 Score)
{
    CurrentScore = Score;

    if (TotalScore)
    {
        TotalScore->SetText(FText::Format(FText::FromString("Score : {0}"), Score));
    }
}

void UMainHUD::UpdateKills(int32 kills)
{
    CurrentKills = kills;

    if (TotalKills)
    {
        TotalKills->SetText(FText::Format(FText::FromString("Kill : {0}"), kills));
    }
}

void UMainHUD::UpdateHealth(float Health)
{
    TargetHealth = Health;
}

void UMainHUD::UpdateStamina(float Stamina)
{
    /*if (PB_Stamina)
    {
        PB_Stamina->SetPercent(Stamina);

        if (Stamina <= 0.3f)
        {
            PB_Stamina->SetFillColorAndOpacity(FLinearColor::Red);
        }
        else if (Stamina <= 0.6f)
        {
            PB_Stamina->SetFillColorAndOpacity(FLinearColor::Yellow);
        }
        else
        {
            PB_Stamina->SetFillColorAndOpacity(FLinearColor::Green);
        }
    }*/
    TargetStamina = Stamina;
}

void UMainHUD::UpdateWhiteKarma(float WhiteKarma)
{
   /* if (PB_WhiteKarma)
    {
        PB_WhiteKarma->SetPercent(WhiteKarma);
    }*/
    TargetWhiteKarma = WhiteKarma;
}

void UMainHUD::UpdateBlackKarma(float BlackKarma)
{
    /*if (PB_BlackKarma)
    {
        PB_BlackKarma->SetPercent(BlackKarma);
    }*/
    TargetBlackKarma = BlackKarma;
}

void UMainHUD::UpdateTime(float Time)
{
    if (Timer)
    {
        int32 Minutes = FMath::FloorToInt(Time / 60.0f);
        int32 Seconds = FMath::FloorToInt(FMath::Fmod(Time, 60.0f));

        FString TimeString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
        Timer->SetText(FText::FromString(TimeString));

        // 10초 미만일 때 빨간색 연출은 유지
        if (Time <= 10.0f)
        {
            Timer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
        }
        else
        {
            Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }
    }
}

void UMainHUD::AddNewQuest(FString Title, FString Desc)
{
    if (QuestList && QuestItemClass)
    {
        UQuestItemWidget* NewItem = CreateWidget<UQuestItemWidget>(GetWorld(), QuestItemClass);
        if (NewItem)
        {
            NewItem->SetQuestText(Title, Desc);
            QuestList->AddChildToVerticalBox(NewItem);

            // [추가된 로직] 퀘스트 리스트에 항목이 추가되었으므로 아이콘을 보여줍니다.
            if (img_Quest)
            {
                img_Quest->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }
}

void UMainHUD::FinishQuest(FString Title)
{
    if (!QuestList) return;

    TArray<UWidget*> Quests = QuestList->GetAllChildren();
    for (UWidget* Widget : Quests)
    {
        UQuestItemWidget* QuestItem = Cast<UQuestItemWidget>(Widget);
        if (QuestItem)
        {
            if (QuestItem->GetQuestTitle().Equals(Title))
            {
                // 1. 우선 퀘스트를 완료 상태(초록색 등)로 변경합니다.
                QuestItem->CompleteQuest();

                // 2. 3초 뒤에 실제로 리스트에서 제거하고 아이콘을 체크하는 람다 함수를 실행합니다.
                FTimerHandle TimerHandle;

                // [이 시점의 Widget과 QuestList를 캡처하여 3초 뒤 실행]
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Widget]()
                    {
                        if (QuestList && Widget)
                        {
                            // 3. 리스트에서 위젯 제거
                            QuestList->RemoveChild(Widget);

                            // 4. 남아있는 퀘스트가 없다면 아이콘 숨기기
                            if (QuestList->GetChildrenCount() == 0)
                            {
                                if (img_Quest)
                                {
                                    img_Quest->SetVisibility(ESlateVisibility::Collapsed);
                                }
                            }
                        }
                    }, 3.0f, false); // 3.0f 가 3초를 의미합니다.

                break;
            }
        }
    }
}

void UMainHUD::ShowWaveResult(bool bSuccess, int32 Bonus, int32 InScore, int32 InKills)
{
    if (ResultWidgetClass)
    {
        UResultWidget* ResultUI = CreateWidget<UResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultUI)
        {
            ResultUI->AddToViewport(10);
            if (bSuccess)
            {
                // 성공 시: 기존처럼 데이터 전달 (Txt_ResultTitle은 기본값이 SUCCESS라고 가정)
                ResultUI->SetupSuccess(InKills, InScore, Bonus);
            }
            else
            {
                // 실패 시: 방금 만드신 실패 전용 함수 호출 (텍스트 숨기기 및 빨간색 변경)
                ResultUI->SetupFailure();
            }

            FTimerHandle ScoreUpdateHandle;
            // 람다 캡처에 bSuccess를 추가합니다.
            GetWorld()->GetTimerManager().SetTimer(ScoreUpdateHandle, [this, ResultUI, Bonus, bSuccess]()
                {
                    if (ResultUI)
                    {
                        ResultUI->RemoveFromParent();

                        // [수정] 성공했을 때만 보너스 점수를 합산합니다.
                        if (bSuccess)
                        {
                            CurrentScore += Bonus;
                            UpdateScore(CurrentScore);
                        }

                    }
                }, 3.0f, false);
        }
    }
}

void UMainHUD::SetQuestIconVisibility(bool bVisible)
{
    if (img_Quest)
    {
        ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
        img_Quest->SetVisibility(NewVisibility);
    }
}

void UMainHUD::RefreshQuestIcon()
{
    if (img_Quest)
    {
        // 퀘스트 개수가 0보다 크면 Visible, 아니면 Collapsed(공간까지 제거)
        if (CurrentQuestCount > 0)
        {
            img_Quest->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            img_Quest->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UMainHUD::RemoveQuest()
{
    // 1. 현재 QuestList에 남아있는 퀘스트 위젯의 개수를 확인합니다.
    // 만약 리스트에 자식 위젯이 하나도 없다면 (0개라면)
    if (QuestList && QuestList->GetChildrenCount() == 0)
    {
        if (img_Quest)
        {
            // 2. 이미지를 다시 숨깁니다.
            img_Quest->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UMainHUD::StartWaveUI(FString Message, float WaveTime)
{
    TempWaveScore = CurrentScore;
    TempWaveKills = CurrentKills;

    CurrentRemainingTime = WaveTime;
    bIsTimerActive = true;

    if (Timer) Timer->SetVisibility(ESlateVisibility::Visible);

    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetText(FText::FromString(Message));
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Visible);

        FTimerHandle MsgHandle;
        GetWorld()->GetTimerManager().SetTimer(MsgHandle, [this]()
            {
                if (Txt_WaveMessage) Txt_WaveMessage->SetVisibility(ESlateVisibility::Collapsed);
            }, 2.0f, false);
    }
}

void UMainHUD::HideWaveMessage()
{
    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UMainHUD::EndWaveUI(bool bSuccess, int32 Score, int32 Bonus, int32 Kills)
{
    if (Timer) Timer->SetVisibility(ESlateVisibility::Collapsed);

    

    // 결과창 위젯 띄우기
    ShowWaveResult(bSuccess, Bonus, Score, Kills);
}