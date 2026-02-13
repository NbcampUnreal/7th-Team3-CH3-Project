#include "UI/QuestItemWidget.h"
#include "Components/TextBlock.h"

void UQuestItemWidget::SetQuestText(FString Title, FString Description)
{
    if (Txt_QuestTitle)
    {
        Txt_QuestTitle->SetText(FText::FromString(Title));
    }

    if (Txt_QuestDesc)
    {
        Txt_QuestDesc->SetText(FText::FromString(Description));
    }
}

void UQuestItemWidget::CompleteQuest()
{
    if (bIsCompleted)
    {
        return;
    }
    bIsCompleted = true;

    if (Txt_QuestTitle)
    {
        Txt_QuestTitle->SetColorAndOpacity(FLinearColor::Green);
    }
    if (Txt_QuestDesc)
    {
        Txt_QuestDesc->SetColorAndOpacity(FLinearColor::Green);
    }

    GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &UQuestItemWidget::DestroyQuestItem, 3.0f, false);
}

void UQuestItemWidget::DestroyQuestItem()
{
    RemoveFromParent();
}

FString UQuestItemWidget::GetQuestTitle() const
{
    if (Txt_QuestTitle)
    {
        return Txt_QuestTitle->GetText().ToString();
    }
    return FString();
}