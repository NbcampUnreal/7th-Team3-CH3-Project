#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestItemWidget.generated.h"

UCLASS()
class TEAM3_PROJECT_API UQuestItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_QuestTitle;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_QuestDesc;

public:
    void SetQuestText(int32 ID, FString Title, FString Description);
    void CompleteQuest();
    FString GetQuestTitle() const;

private:
    bool bIsCompleted = false;

    void DestroyQuestItem();

    FTimerHandle DestroyTimerHandle;
    int32 QuestID;
};
