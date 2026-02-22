#include "UI/ResultWidget.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UResultWidget::SetupSuccess(int32 InKills, int32 InScore, int32 InBonus)
{
    // 가시성 확보 (실패 후 재도전 시 필요)
    if (Txt_KillCount) Txt_KillCount->SetVisibility(ESlateVisibility::Visible);
    if (Txt_BaseScore) Txt_BaseScore->SetVisibility(ESlateVisibility::Visible);
    if (Txt_BonusScore) Txt_BonusScore->SetVisibility(ESlateVisibility::Visible);

    // [수정] 숫자 앞에 "Kill : " 같은 설명 문구를 붙여서 표시합니다.
    if (Txt_KillCount)
        Txt_KillCount->SetText(FText::Format(FText::FromString("Kill : {0}"), InKills));

    if (Txt_BaseScore)
        Txt_BaseScore->SetText(FText::Format(FText::FromString("Score : {0}"), InScore));

    if (Txt_BonusScore)
        Txt_BonusScore->SetText(FText::Format(FText::FromString("Bonus Score : {0}"), InBonus));

    if (Txt_ResultTitle)
    {
        Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE SUCCESS")));
        Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Green); // 성공이니까 초록색이나 흰색으로!
    }
}

void UResultWidget::SetupFailure()
{
    Txt_ResultTitle->SetText(FText::FromString(TEXT("WAVE FAILED")));
    Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Red); 

    Txt_KillCount->SetVisibility(ESlateVisibility::Collapsed);
    Txt_BonusScore->SetVisibility(ESlateVisibility::Collapsed);
    Txt_BaseScore->SetVisibility(ESlateVisibility::Collapsed);
}

void UResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsCounting)
    {
        // 1. 현재 표시용 보너스 점수를 목표치로 부드럽게 이동
        CurrentDisplayBonus = FMath::FInterpTo(CurrentDisplayBonus, TargetBonus, InDeltaTime, 2.0f);

        if (Txt_BonusScore)
        {
            // 반올림하여 정수로 표시
            Txt_BonusScore->SetText(FText::AsNumber(FMath::RoundToInt(CurrentDisplayBonus)));
        }

        // 2. [수정] 목표치와 차이가 1 미만이면 종료 처리
        if (FMath::Abs(CurrentDisplayBonus - TargetBonus) < 1.0f)
        {
            CurrentDisplayBonus = TargetBonus; // 최종값 강제 고정
            if (Txt_BonusScore) Txt_BonusScore->SetText(FText::AsNumber(FMath::RoundToInt(TargetBonus)));

            bIsCounting = false; // 여기서 Tick 로직이 멈춥니다.
        }
    }
}

