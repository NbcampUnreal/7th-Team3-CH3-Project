#include "UI/QuestItemWidget.h"
#include "Components/TextBlock.h"

void UQuestItemWidget::SetQuestText(int32 ID, FString Title, FString Description)
{
    QuestID = ID; // HUD에서 관리하기 위한 ID 저장

    if (Txt_QuestTitle) Txt_QuestTitle->SetText(FText::FromString(Title));
    if (Txt_QuestDesc) Txt_QuestDesc->SetText(FText::FromString(Description));
}

void UQuestItemWidget::CompleteQuest()
{
    if (bIsCompleted) return;
    bIsCompleted = true;

    // 완료 시 시각적 피드백 (녹색)
    if (Txt_QuestTitle) Txt_QuestTitle->SetColorAndOpacity(FLinearColor::Green);
    if (Txt_QuestDesc) Txt_QuestDesc->SetColorAndOpacity(FLinearColor::Green);

    // 3초 뒤 자동 삭제 
    GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &UQuestItemWidget::DestroyQuestItem, 3.0f, false);
}

void UQuestItemWidget::DestroyQuestItem()
{
    RemoveFromParent();
}