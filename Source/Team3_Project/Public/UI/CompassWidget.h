#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CompassWidget.generated.h"

UCLASS()
class TEAM3_PROJECT_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) UImage* OuterCompass;
	UPROPERTY(meta = (BindWidget)) UImage* InnerNeedle;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
