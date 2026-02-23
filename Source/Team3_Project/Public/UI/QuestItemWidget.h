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
    // 퀘스트 정보 설정 (ID 포함) 
    void SetQuestText(int32 ID, FString Title, FString Description);

    // 퀘스트 완료 처리 (색상 변경 및 삭제 예약) 
    void CompleteQuest();

    // 현재 퀘스트 ID 반환 
    int32 GetQuestID() const { return QuestID; }

private:
    // 퀘스트를 리스트에서 제거 
    void DestroyQuestItem();

    int32 QuestID;
    bool bIsCompleted = false;
    FTimerHandle DestroyTimerHandle;
};
