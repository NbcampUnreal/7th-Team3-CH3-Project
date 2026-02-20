// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryItemSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "UI/Inventory/InventoryDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventoryMainWidget.h"
#include "Item/InventoryComponent.h"

void UInventoryItemSlot::InitSlot(const FInventoryItem& Item)
{

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	// 슬롯이 표시할 ItemID 저장
	CurrentItemID = Item.ItemID;
	CurrentQuantity = Item.Quantity;

	// 수량 텍스트 표시
	if (ItemQuantity)
	{
		if (Item.Quantity > 1)
		{
			ItemQuantity->SetText(FText::AsNumber(Item.Quantity));
			ItemQuantity->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitSlot: ItemQuantity widget is null"));
	}

	// 아이콘 표시(아이템 데이터에서 아이콘 로딩)
	if (ItemIcon)
	{
		FItemData ItemData = ItemDataSubsystem->GetItemDataByID(Item.ItemID);
		UTexture2D* IconTexture = ItemData.Icon.LoadSynchronous();
		if (IconTexture)
		{
			ItemIcon->SetBrushFromTexture(IconTexture);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
			if (SlotButton)
			{
				SlotButton->SetIsEnabled(true);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("InitSlot: SlotButton is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InitSlot: IconTexture is null for ItemID=%s"), *Item.ItemID.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitSlot: ItemIcon widget is null"));
	}
}

void UInventoryItemSlot::ClearSlot()
{
	// 슬롯을 빈 상태로 초기화
	CurrentItemID = NAME_None;
	CurrentQuantity = 0;

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearSlot: ItemIcon widget is null"));
	}
	if (ItemQuantity)
	{
		ItemQuantity->SetText(FText::GetEmpty());
		ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearSlot: ItemQuantity widget is null"));
	}
	if (SlotButton)
	{
		SlotButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearSlot: SlotButton widget is null"));
	}
}

void UInventoryItemSlot::SetSelectedSlot(bool bSelected)
{
	// 선택 표시(테두리 표시/숨김)
	UE_LOG(LogTemp, Warning, TEXT("SetSelectedSlot: Slot=%s Selected=%s"), *GetNameSafe(this), bSelected ? TEXT("true") : TEXT("false"));
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSelectedSlot: SelectionBorder widget is null"));
	}
}

void UInventoryItemSlot::SetSlotType(ESlotType NewType, int32 NewIndex)
{
	// 슬롯 목적(인벤토리/부착/퀵슬롯)과 인덱스 저장
	SlotType = NewType;
	SlotIndex = NewIndex;
}

void UInventoryItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// 아이템이 있을 때만 부모 위젯에 Hover 이벤트 전달(툴팁 표시)
	if (!CurrentItemID.IsNone() && ParentInventoryWidget)
	{
		FInventoryItem DummyItem;
		DummyItem.ItemID = CurrentItemID;
		ParentInventoryWidget->HandleSlotHover(this, DummyItem);
	}
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	// 부모 위젯에 Unhover 이벤트 전달(툴팁 숨김)
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->HandleSlotUnhover(this);
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// 우클릭: 우클릭 메뉴(사용/드랍 등)를 열기 위해 부모 위젯에 전달
		if (!CurrentItemID.IsNone() && ParentInventoryWidget)
		{
			FInventoryItem DummyItem;
			DummyItem.ItemID = CurrentItemID;
			DummyItem.Quantity = CurrentQuantity;

			ParentInventoryWidget->HandleSlotRightClick(this, DummyItem);
			return FReply::Handled();
		}
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// 좌클릭: 선택 처리 후 드래그 감지 시작
		if (ParentInventoryWidget)
		{
			ParentInventoryWidget->HandleSlotSelection(this);
		}
		else
		{
			SetSelectedSlot(true);
		}

		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected: Slot=%s ItemID=%s"), *GetNameSafe(this), *CurrentItemID.ToString());

	// 빈 슬롯은 드래그 불가
	if (CurrentItemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected: Aborted (no item)"));
		return;
	}

	// 드래그 중 전달할 데이터(Operation) 생성
	UInventoryDragDropOperation* DragDropOp = NewObject<UInventoryDragDropOperation>();
	DragDropOp->ItemID = CurrentItemID;
	DragDropOp->SourceIndex = SlotIndex;
	DragDropOp->SourceType = static_cast<uint8>(SlotType);
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected: Created operation ItemID=%s SourceIndex=%d SourceType=%d"), *CurrentItemID.ToString(), SlotIndex, static_cast<int32>(SlotType));

	// 드래그 중 커서 아래에 보여줄 시각 요소(슬롯 위젯을 재사용)
	UInventoryItemSlot* DragVisual = CreateWidget<UInventoryItemSlot>(this, this->GetClass());
	if (DragVisual)
	{
		FInventoryItem DragItem;
		DragItem.ItemID = CurrentItemID;
		DragItem.Quantity = 1; // 드래그 시 1개만 드래그한다고 가정
		DragVisual->InitSlot(DragItem);

		DragDropOp->DefaultDragVisual = DragVisual;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected: Failed to create drag visual"));
	}
	DragDropOp->Pivot = EDragPivot::CenterCenter;

	OutOperation = DragDropOp;
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected: OutOperation set"));
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 드롭 시 전달된 Operation을 확인하고, 슬롯 타입 조합에 따라 동작 분기
	UInventoryDragDropOperation* DragDropOp = Cast<UInventoryDragDropOperation>(InOperation);
	if (!DragDropOp)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Invalid operation (not UInventoryDragDropOperation)"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: TargetSlot=%s TargetType=%d TargetIndex=%d SourceType=%d SourceIndex=%d ItemID=%s"),
		*GetNameSafe(this),
		static_cast<int32>(SlotType),
		SlotIndex,
		static_cast<int32>(DragDropOp->SourceType),
		DragDropOp->SourceIndex,
		*DragDropOp->ItemID.ToString());

	if (!ParentInventoryWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: ParentInventoryWidget is null"));
		return false;
	}

	UInventoryComponent* InventoryComponent = ParentInventoryWidget->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: InventoryComponent is null"));
		return false;
	}

	// 인벤토리 슬롯 -> 인벤토리 슬롯 : 아이템 위치 교환
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Inventory) && SlotType == ESlotType::ST_Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Swapping inventory items (%d <-> %d)"), DragDropOp->SourceIndex, SlotIndex);
		InventoryComponent->SwapInventoryItems(DragDropOp->SourceIndex, SlotIndex);
		return true;
	}

	// 인벤토리 슬롯 -> 부착 슬롯 : 해당 부착 부위로 장착
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Inventory) && SlotType == ESlotType::ST_Attachment)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Equipping attachment from inventory slot=%d to attachmentType=%d"), DragDropOp->SourceIndex, SlotIndex);
		InventoryComponent->EquipAttachmentFromSlot(DragDropOp->SourceIndex, static_cast<EAttachmentType>(SlotIndex));
		return true;
	}

	// 부착 슬롯 -> 인벤토리 슬롯 : 인벤토리의 특정 칸으로 해제/이동(또는 교환)
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Attachment) && SlotType == ESlotType::ST_Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Unequipping attachmentType=%d to inventory slot=%d"), DragDropOp->SourceIndex, SlotIndex);
		InventoryComponent->UnequipAttachmentToSlot(static_cast<EAttachmentType>(DragDropOp->SourceIndex), SlotIndex);
		return true;
	}

	// 인벤토리 슬롯 -> 퀵슬롯 : 해당 퀵슬롯에 아이템 배정
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Inventory) && SlotType == ESlotType::ST_QuickSlot)
	{
		if (InventoryComponent->GetInventoryContents().IsValidIndex(DragDropOp->SourceIndex))
		{
			FName TargetItemID = InventoryComponent->GetInventoryContents()[DragDropOp->SourceIndex].ItemID;
			UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Assigning inventory item %s to quickslot=%d"), *TargetItemID.ToString(), SlotIndex);
			InventoryComponent->AssignToQuickSlot(SlotIndex, TargetItemID);
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Source inventory index out of range (%d)"), DragDropOp->SourceIndex);
	}

	// 퀵슬롯 <-> 퀵슬롯 : 서로 배정 아이템 교환
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_QuickSlot) && SlotType == ESlotType::ST_QuickSlot)
	{
		FName SourceItemID = InventoryComponent->QuickSlots[DragDropOp->SourceIndex];
		FName TargetItemID = InventoryComponent->QuickSlots[SlotIndex];
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Swapping quickslots (%d:%s <-> %d:%s)"), DragDropOp->SourceIndex, *SourceItemID.ToString(), SlotIndex, *TargetItemID.ToString());
		InventoryComponent->AssignToQuickSlot(SlotIndex, SourceItemID);
		InventoryComponent->AssignToQuickSlot(DragDropOp->SourceIndex, TargetItemID);
		return true;
	}

	// 인벤토리 슬롯 -> 무기/방어구 슬롯 : 장착/해제 또는 교환
	if (DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Inventory) && (SlotType == ESlotType::ST_Weapon || SlotType == ESlotType::ST_Armor))
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Handling inventory -> equipment slot interaction (SourceIndex=%d TargetType=%d TargetIndex=%d)"), DragDropOp->SourceIndex, static_cast<int32>(SlotType), SlotIndex);
		InventoryComponent->EquipItemFromInventory(DragDropOp->SourceIndex, SlotType);
		return true;
	}

	// 무기/방어구 슬롯 -> 인벤토리 슬롯 : 인벤토리로 해제/이동(또는 교환)
	if ((DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Weapon) || DragDropOp->SourceType == static_cast<uint8>(ESlotType::ST_Armor)) && SlotType == ESlotType::ST_Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Handling equipment -> inventory slot interaction (SourceType=%d SourceIndex=%d TargetIndex=%d)"), static_cast<int32>(DragDropOp->SourceType), DragDropOp->SourceIndex, SlotIndex);
		InventoryComponent->UnequipItemToInventory(static_cast<ESlotType>(DragDropOp->SourceType), SlotIndex);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: No matching drop handler"));
	return false;
}

