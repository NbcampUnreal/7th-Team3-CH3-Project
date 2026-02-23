#include "UI/ResultWidget.h"
#include "Components/TextBlock.h"

void UResultWidget::SetupSuccess(int32 InKills, int32 InScore, int32 InBonus, bool bSuccess)
{
    if (bSuccess)
    {
        if (Txt_KillCount) 
        {
            Txt_KillCount->SetVisibility(ESlateVisibility::Visible);
            Txt_KillCount->SetText(FText::Format(FText::FromString(TEXT("Kill : {0}")), InKills));
        }
        if (Txt_BaseScore) 
        {
            Txt_BaseScore->SetVisibility(ESlateVisibility::Visible);
            Txt_BaseScore->SetText(FText::Format(FText::FromString(TEXT("Score : {0}")), InScore));
        }
        if (Txt_BonusScore) 
        {
            Txt_BonusScore->SetVisibility(ESlateVisibility::Visible);
            Txt_BonusScore->SetText(FText::Format(FText::FromString(TEXT("Bonus Score : {0}")), InBonus));
        }

        if (Txt_ResultTitle) 
        {
            Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE SUCCESS")));
            Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Green);
        }
    }
    else
    {
        SetupFailure();
    }

    FTimerHandle DestoryHandle;
    GetWorld()->GetTimerManager().SetTimer(DestoryHandle, this, &UResultWidget::RemoveFromParent, 3.0f, false);
}

void UResultWidget::SetupFailure()
{
    if (Txt_ResultTitle)
    {
        Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE FAILED")));
        Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Red);
    }

    // 실패 시 상세 성적 숨기기
    if (Txt_KillCount) Txt_KillCount->SetVisibility(ESlateVisibility::Collapsed);
    if (Txt_BonusScore) Txt_BonusScore->SetVisibility(ESlateVisibility::Collapsed);
    if (Txt_BaseScore) Txt_BaseScore->SetVisibility(ESlateVisibility::Collapsed);
}

void UResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsCounting)
    {

        CurrentDisplayBonus = FMath::FInterpTo(CurrentDisplayBonus, TargetBonus, InDeltaTime, 2.0f);

        if (Txt_BonusScore)
        {
            Txt_BonusScore->SetText(FText::Format(FText::FromString("Bonus Score : {0}"), FMath::RoundToInt(CurrentDisplayBonus)));
        }

        if (FMath::Abs(CurrentDisplayBonus - TargetBonus) < 1.0f)
        {
            CurrentDisplayBonus = TargetBonus;
            bIsCounting = false;
        }
    }
}