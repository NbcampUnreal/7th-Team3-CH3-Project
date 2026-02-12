#include "UI/MainHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

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
}

void UMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Collapsed);
    }
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
    if (TotalScore)
    {
        TotalScore->SetText(FText::Format(FText::FromString("Score : {0}"), Score));
    }
}

void UMainHUD::UpdateKills(int32 kills)
{
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
        if (Timer->GetVisibility() != ESlateVisibility::Visible)
        {
            Timer->SetVisibility(ESlateVisibility::Visible);
        }

        int32 Minutes = FMath::FloorToInt(Time / 60.0f);
        int32 Seconds = FMath::FloorToInt(FMath::Fmod(Time, 60.0f));

        FString TimeString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);

        Timer->SetText(FText::FromString(TimeString));

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