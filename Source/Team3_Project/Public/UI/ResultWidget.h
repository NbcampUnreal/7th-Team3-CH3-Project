#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

UCLASS()
class TEAM3_PROJECT_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// ResultWidget(const FObjectInitializer& ObjectInitializer);
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_ResultTitle;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_KillCount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_BonusScore;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_BaseScore;

	float TargetBonus;
	float CurrentDisplayBonus;
	int32 BaseScore;
	bool bIsCounting=false;
	int32 FinalBonus;
public:
	void SetupSuccess(int32 InBaseScore, int32 InBonus, int32 InKills);
	void SetupFailure();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
