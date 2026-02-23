// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryActionMenu.h"
#include "Components/Button.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Spacer.h"
#include "UI/Inventory/InventoryMainWidget.h"

void UInventoryActionMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 위젯 생성 시 버튼 이벤트 바인딩
	UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu::NativeOnInitialized"));
	if (Btn_Use)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Binding Use button"));
		// "사용" 버튼 클릭 시 아이템 사용 요청
		Btn_Use->OnClicked.AddDynamic(this, &UInventoryActionMenu::OnUseClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Btn_Use is null"));
	}
	if (Btn_Drop)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Binding Drop button"));
		// "버리기" 버튼 클릭 시 아이템 제거 요청
		Btn_Drop->OnClicked.AddDynamic(this, &UInventoryActionMenu::OnDropClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Btn_Drop is null"));
	}
}

void UInventoryActionMenu::Setup(
	const FInventoryItem& Item,
	UInventoryComponent* InventoryComp,
	ESlotType SlotType,
	int32 SlotIndex,
	UInventoryMainWidget* ParentWidget)
{
	// 컨텍스트 메뉴가 조작할 대상(아이템/인벤토리)을 저장
	UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu::Setup: ItemID=%s Qty=%d InventoryComp=%s SlotType=%d SlotIndex=%d"), *Item.ItemID.ToString(), Item.Quantity, *GetNameSafe(InventoryComp), static_cast<int32>(SlotType), SlotIndex);
	TargetItem = Item;
	LinkedInventoryComp = InventoryComp;
	ParentInventoryWidget = ParentWidget;
	CurrentSlotType = SlotType;
	CurrentSlotIndex = SlotIndex;

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	FItemData Data = ItemDataSubsystem->GetItemDataByID(TargetItem.ItemID);

	Btn_Use->SetVisibility(ESlateVisibility::Visible);
	Btn_Drop->SetVisibility(ESlateVisibility::Visible);

	UTextBlock* Txt_Use = Cast<UTextBlock>(Btn_Use->GetChildAt(0));
	UTextBlock* Txt_Drop = Cast<UTextBlock>(Btn_Drop->GetChildAt(0));

	if (SlotType == ESlotType::ST_QuickSlot ||
		SlotType == ESlotType::ST_Attachment ||
		SlotType == ESlotType::ST_Weapon ||
		SlotType == ESlotType::ST_Armor)
	{
		if (Txt_Use)
		{
			Txt_Use->SetText(FText::FromString(TEXT("해제")));
		}
		Btn_Drop->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		if (Data.ItemType == EItemType::IT_Ammo ||
			Data.ItemType == EItemType::IT_Ingredient ||
			Data.ItemType == EItemType::IT_QuestItem)
		{
			Btn_Use->SetVisibility(ESlateVisibility::Collapsed);
			if (Txt_Drop)
			{
				Txt_Drop->SetText(FText::FromString(TEXT("버리기")));
			}
		}
		else if (Data.ItemType == EItemType::IT_Weapon ||
				 Data.ItemType == EItemType::IT_Armor ||
				 Data.ItemType == EItemType::IT_Attachment)
		{
			if (Txt_Use)
			{
				Txt_Use->SetText(FText::FromString(TEXT("장착")));
			}
			if (Txt_Drop)
			{
				Txt_Drop->SetText(FText::FromString(TEXT("버리기")));
			}
		}
		else
		{
			if (Txt_Use)
			{
				Txt_Use->SetText(FText::FromString(TEXT("사용")));
			}
			if (Txt_Drop)
			{
				Txt_Drop->SetText(FText::FromString(TEXT("버리기")));
			}
		}
	}

	if (ActionSpacer)
	{
		bool bShowSpacer = (Btn_Use->GetVisibility() == ESlateVisibility::Visible) &&
						   (Btn_Drop->GetVisibility() == ESlateVisibility::Visible);
		if (bShowSpacer)
		{
			ActionSpacer->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ActionSpacer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryActionMenu::OnUseClicked()
{
	// 선택된 아이템을 사용(장비/소모 등은 RequestUseItem에서 분기)
	if (!LinkedInventoryComp)
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	FItemData Data = ItemDataSubsystem->GetItemDataByID(TargetItem.ItemID);

	UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu::OnUseClicked: ItemID=%s"), *TargetItem.ItemID.ToString());

	if (!TargetItem.ItemID.IsNone())
	{
		if (CurrentSlotType == ESlotType::ST_QuickSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Unassign from quick slot index=%d"), CurrentSlotIndex);
			LinkedInventoryComp->AssignToQuickSlot(CurrentSlotIndex, NAME_None);
		}
		else if (CurrentSlotType == ESlotType::ST_Attachment)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Unequip attachment from slot index=%d"), CurrentSlotIndex);
			LinkedInventoryComp->UnequipAttachmentToSlot(static_cast<EAttachmentType>(CurrentSlotIndex), -1);
		}
		else if (CurrentSlotType == ESlotType::ST_Weapon || CurrentSlotType == ESlotType::ST_Armor)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: Unequip item from slot index=%d"), CurrentSlotIndex);
			LinkedInventoryComp->UnequipItemToInventory(CurrentSlotType, -1);
		}
		else
		{
			LinkedInventoryComp->RequestUseItem(TargetItem.ItemID);
			UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu: RequestUseItem ItemID=%s"), *TargetItem.ItemID.ToString());
		}
	}

	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->CloseContextMenu();	
	}
	else
	{
		RemoveFromParent();
	}
}

void UInventoryActionMenu::OnDropClicked()
{
	// 선택된 아이템을 인벤토리에서 제거
	if (!LinkedInventoryComp)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("InventoryActionMenu::OnDropClicked: ItemID=%s"), *TargetItem.ItemID.ToString());
	LinkedInventoryComp->DropItem(TargetItem.ItemID, TargetItem.Quantity, CurrentSlotIndex);

	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->CloseContextMenu();	
	}
	else
	{
		RemoveFromParent();
	}
}