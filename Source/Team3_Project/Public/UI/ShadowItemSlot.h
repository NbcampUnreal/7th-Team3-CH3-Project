#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowItemSlot.generated.h"


UCLASS()
class TEAM3_PROJECT_API UShadowItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Shadow|Slot")
	void UpdateSlotInfo(FName ItemID, int32 NewCount, class UTexture2D* NewIcon);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow|Slot")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow|Slot")
	int32 ItemCount;
};
