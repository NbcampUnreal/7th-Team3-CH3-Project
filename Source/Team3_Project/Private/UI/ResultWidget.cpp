#include "UI/ResultWidget.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UResultWidget::SetupSuccess(int32 InBaseScore, int32 InBonus, int32 InKills)
{
    if (Txt_KillCount)
        Txt_KillCount->SetText(FText::AsNumber(Kills));

    if (Txt_WaveScore)
        Txt_WaveScore->SetText(FText::AsNumber(Score));

    if (Txt_BonusScore)
        Txt_BonusScore->SetText(FText::AsNumber(Bonus));

    if (Txt_ResultTitle)
    {
        FString ResultStr = bIsSuccess ? TEXT("WAVE CLEAR") : TEXT("WAVE FAILED");
        Txt_ResultTitle->SetText(FText::FromString(ResultStr));

        // 성공은 파랑, 실패는 빨강
        FLinearColor TitleColor = bIsSuccess ? FLinearColor::Blue : FLinearColor::Red;
        Txt_ResultTitle->SetColorAndOpacity(TitleColor);
    }
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