#include "UI/ResultWidget.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UResultWidget::SetupSuccess(int32 InBaseScore, int32 InBonus, int32 InKills)
{
    BaseScore = InBaseScore;
    TargetBonus = (float)InBonus;
    CurrentDisplayBonus = 0.0f;
    bIsCounting = true; 

    Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE CLEAR")));
    Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Green); 
    Txt_KillCount->SetText(FText::AsNumber(InKills));
}

void UResultWidget::SetupFailure()
{
    Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE FAILED")));
    Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Red); 

    Txt_KillCount->SetVisibility(ESlateVisibility::Collapsed);
    Txt_BonusScore->SetVisibility(ESlateVisibility::Collapsed);
    Txt_TotalScore->SetVisibility(ESlateVisibility::Collapsed);
}

void UResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsCounting)
    {
        CurrentDisplayBonus = FMath::FInterpTo(CurrentDisplayBonus, TargetBonus, InDeltaTime, 5.0f);

        int32 DisplayBonusInt = FMath::RoundToInt(CurrentDisplayBonus);
        Txt_BonusScore->SetText(FText::AsNumber(DisplayBonusInt));

        Txt_TotalScore->SetText(FText::AsNumber(BaseScore + DisplayBonusInt));

        if (FMath::IsNearlyEqual(CurrentDisplayBonus, TargetBonus, 1.0f))
        {
            Txt_BonusScore->SetText(FText::AsNumber((int32)TargetBonus));
            Txt_TotalScore->SetText(FText::AsNumber(BaseScore + (int32)TargetBonus));
            bIsCounting = false;
        }
    }
}