#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

UCLASS()
class TEAM3_PROJECT_API UResultWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_ResultTitle;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_KillCount;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_BonusScore;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_BaseScore;

    float TargetBonus;
    float CurrentDisplayBonus;
    bool bIsCounting = false;

public:
    // 성공 시 호출
    void SetupSuccess(int32 InKills, int32 InScore, int32 InBonus, bool bSuccess);

    // 실패 시 호출 (텍스트 숨기기 및 경고 표시)
    void SetupFailure();

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
