// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/QuestTableWidget.h"
#include "Core/MainGameState.h"
#include "Item/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UQuestTableWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//기본적으로 버튼 비활성화 해놓는 부분인데 아이템 미구현이라 주석처리 해놨습니다.
	//BT_Accept->SetIsEnabled(false);

	if (BT_Accept)
	{
		BT_Accept->OnClicked.AddDynamic(this, &UQuestTableWidget::OnClickedAccept);
	}

	if (BT_Close)
	{
		BT_Close->OnClicked.AddDynamic(this, &UQuestTableWidget::OnClickedClose);
	}
}

void UQuestTableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* CurrentPlayerController = GetOwningPlayer())
	{
		if (APawn* CurrentPawn = Cast<APawn>(CurrentPlayerController->GetPawn()))
		{
			Inventory = CurrentPawn->FindComponentByClass<UInventoryComponent>();
		}
	}

	if (!Inventory) return;

	//TODO_@Core : 인벤 컴포넌트에서 bool 인자를 가진 퀘스트 아이템 획득 델리게이트 수신받기; 
}

void UQuestTableWidget::OnClickedAccept()
{
	//이 버튼 자체에 조건이 걸려있으면 따로 조건문을 만들어서 전달할 필요가 없다.
	//TODO_@Core : 이벤트 발생 호출
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	if (CurrentGameState == nullptr) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC == nullptr) return;

	CurrentGameState->WaveStart();

	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;
	RemoveFromParent();
}

void UQuestTableWidget::OnClickedClose()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC == nullptr) return;

	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;
	RemoveFromParent();
}


//인벤토리에 퀘스트에 필요한 아이템 들어왔을경우 버튼 활성화 시켜주는 함수
void UQuestTableWidget::UpdateButton(bool bHasQuestItem)
{
	if (bHasQuestItem == false) return;

	BT_Accept->SetIsEnabled(bHasQuestItem);
}
