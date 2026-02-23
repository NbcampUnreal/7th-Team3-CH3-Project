// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryMainWidget.h"
#include "Components/UniformGridPanel.h"
#include "Item/InventoryComponent.h"
#include "UI/Inventory/InventoryItemSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Input/Reply.h"
#include "Item/WeaponItem.h"
#include "Components/Image.h"
#include "Core/ItemDataSubsystem.h"
#include "UI/Inventory/InventoryDragDropOperation.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "UI/Inventory/InventoryActionMenu.h"
#include "UI/Inventory/InventoryTooltip.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void UInventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Begin"));

	// 위젯 생성 시 인벤토리 컴포넌트 자동 탐색(미설정 상태라면)
	if (!InventoryComp)
	{
		APawn* OwnerPawn = GetOwningPlayerPawn();
		if (OwnerPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Searching InventoryComponent on owning pawn=%s"), *GetNameSafe(OwnerPawn));
			UInventoryComponent* FoundComp = OwnerPawn->FindComponentByClass<UInventoryComponent>();
			if (FoundComp)
			{
				InitializeInventory(FoundComp);
				UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: InventoryComponent initialized successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: No InventoryComponent found on owning pawn"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Owning pawn is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: InventoryComp already set (%s), refreshing"), *GetNameSafe(InventoryComp));
		RefreshInventory();
	}

	if (TooltipWidgetClass && !ActiveTooltip)
	{
		// 툴팁 위젯은 한 번 생성해두고 필요할 때만 보이도록 처리
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Creating tooltip widget"));
		ActiveTooltip = CreateWidget<UUserWidget>(this, TooltipWidgetClass);
		if (ActiveTooltip)
		{
			ActiveTooltip->AddToViewport(100);
			ActiveTooltip->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Failed to create tooltip widget"));
		}
	}

	if (ContextMenuWidgetClass && !ActiveContextMenu)
	{
		// 컨텍스트 메뉴 위젯도 생성해두고 우클릭 시 표시
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Creating context menu widget"));
		ActiveContextMenu = CreateWidget<UInventoryActionMenu>(this, ContextMenuWidgetClass);
		if (ActiveContextMenu)
		{
			ActiveContextMenu->AddToViewport(110);
			ActiveContextMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Failed to create context menu widget"));
		}
	}

	if (Slot_Scope)
	{
		// 무기 부착 슬롯은 고정 타입/인덱스를 부여
		Slot_Scope->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Scope);
		Slot_Scope->ParentInventoryWidget = this;
	}
	if (Slot_Barrel)
	{
		Slot_Barrel->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Barrel);
		Slot_Barrel->ParentInventoryWidget = this;
	}
	if (Slot_Magazine)
	{
		Slot_Magazine->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Magazine);
		Slot_Magazine->ParentInventoryWidget = this;
	}
	if (Slot_Underbarrel)
	{
		Slot_Underbarrel->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Underbarrel);
		Slot_Underbarrel->ParentInventoryWidget = this;
	}
	if (Slot_Stock)
	{
		Slot_Stock->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Stock);
		Slot_Stock->ParentInventoryWidget = this;
	}
	if (Slot_Weapon)
	{
		Slot_Weapon->SetSlotType(ESlotType::ST_Weapon, 0);
		Slot_Weapon->ParentInventoryWidget = this;
	}
	if (Slot_Armor)
	{
		Slot_Armor->SetSlotType(ESlotType::ST_Armor, 0);
		Slot_Armor->ParentInventoryWidget = this;
	}

	if (Img_Weapon)
	{
		if (InventoryComp->GetEquippedWeapon())
		{
			Img_Weapon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_Weapon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Setting input mode to GameAndUI and pausing"));
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(this, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: Owning player controller is null"));
	}

	UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: End"));
}

void UInventoryMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Warning, TEXT("NativeDestruct: Begin"));

	CloseContextMenu();
	HideTooltip();

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeDestruct: Restoring input mode to GameOnly and unpausing"));
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		UGameplayStatics::SetGamePaused(this, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeDestruct: Owning player controller is null"));
	}
	UE_LOG(LogTemp, Warning, TEXT("NativeDestruct: End"));
}












void UInventoryMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// 툴팁이 떠있는 동안에는 마우스 위치를 따라가도록 갱신
	if (ActiveTooltip && ActiveTooltip->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		UpdateTooltipPosition();
	}
}

FReply UInventoryMainWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonDown: ContextMenuOpen=%s"), bIsContextMenuOpen ? TEXT("true") : TEXT("false"));
	// 컨텍스트 메뉴가 열려있을 때 빈 곳 클릭하면 닫기
	if (bIsContextMenuOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonDown: Closing context menu"));
		CloseContextMenu();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryMainWidget::InitializeInventory(UInventoryComponent* NewInventoryComp)
{
	if (!NewInventoryComp)
	{
		return;
	}

	InventoryComp = NewInventoryComp;
	// 인벤토리 업데이트 델리게이트에 바인딩
	InventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryMainWidget::RefreshInventory);

	InventoryComp->OnQuickSlotUpdated.AddDynamic(this, &UInventoryMainWidget::RefreshInventory);

	RefreshInventory();
}






















void UInventoryMainWidget::RefreshInventory()
{
	// 인벤토리/퀵슬롯 UI 전체를 다시 구성(현재 상태를 그대로 다시 그림)
	if (!InventoryComp || !InventoryGrid || !SlotWidgetClass)
	{
		return;
	}
	// 기존 슬롯들 제거
	InventoryGrid->ClearChildren();
	// 인벤토리 아이템 가져오기
	const TArray<FInventoryItem>& Items = InventoryComp->GetInventoryContents();

	int32 TotalSlots = InventoryComp->GetCapacity();
	// 그리드에 슬롯 추가
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		// 새 슬롯 위젯 생성
		UInventoryItemSlot* NewSlot = CreateWidget<UInventoryItemSlot>(this, SlotWidgetClass);
		if (NewSlot)
		{
			NewSlot->ParentInventoryWidget = this;
			NewSlot->SetSlotType(ESlotType::ST_Inventory, i);
			// 아이템이 있으면 초기화, 없으면 비우기
			if (Items.IsValidIndex(i) && !Items[i].ItemID.IsNone())
			{
				NewSlot->InitSlot(Items[i]);
			}
			else
			{
				NewSlot->ClearSlot();
			}

			// 행과 열 계산 후 그리드에 추가
			int32 Row = i / Columns;
			int32 Column = i % Columns;

			InventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RefreshInventory: Failed to create slot widget at index=%d"), i);
		}
	}

	if (QuickSlotPanel)
	{
		// 퀵슬롯 패널도 동일하게 현재 배정 상태로 다시 구성
		QuickSlotPanel->ClearChildren();
		const TArray<FName>& QuickSlots = InventoryComp->QuickSlots;
		for (int32 i = 0; i < QuickSlots.Num(); ++i)
		{
			UInventoryItemSlot* NewSlot = CreateWidget<UInventoryItemSlot>(this, SlotWidgetClass);
			if (NewSlot)
			{
				NewSlot->ParentInventoryWidget = this;
				NewSlot->SetSlotType(ESlotType::ST_QuickSlot, i);
				FName ItemID = QuickSlots[i];
				if (!ItemID.IsNone())
				{
					FInventoryItem QuickSlotItem;
					QuickSlotItem.ItemID = ItemID;
					QuickSlotItem.Quantity = InventoryComp->GetItemQuantity(ItemID);
					NewSlot->InitSlot(QuickSlotItem);
				}
				else
				{
					NewSlot->ClearSlot();
				}
				UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(QuickSlotPanel->AddChild(NewSlot));
				if (HSlot)
				{
					HSlot->SetPadding(FMargin(5.0f, 0.0f, 5.0f, 0.0f));
					HSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RefreshInventory: Failed to create quick slot widget at index=%d"), i);
			}
		}
	}

	// 무기 패널(장착 무기/부착물 슬롯)도 함께 갱신
	UpdateWeaponPanel();
}













void UInventoryMainWidget::HandleSlotHover(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item)
{
	// Hover 시 툴팁 표시(컨텍스트 메뉴가 열려있으면 무시)
	if (bIsContextMenuOpen)
	{
		return;
	}
	ShowTooltip(Item);
}

void UInventoryMainWidget::HandleSlotUnhover(UInventoryItemSlot* ItemSlot)
{
	// Unhover 시 툴팁 숨김
	HideTooltip();
}

void UInventoryMainWidget::HandleSlotRightClick(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item)
{
	// 우클릭 시 툴팁을 내리고 컨텍스트 메뉴를 열어준다
	UE_LOG(LogTemp, Warning, TEXT("HandleSlotRightClick: ItemID=%s Slot=%s"), *Item.ItemID.ToString(), *GetNameSafe(ItemSlot));
	HideTooltip();
	if (ItemSlot)
	{
		OpenContextMenu(Item, ItemSlot->SlotType, ItemSlot->SlotIndex);
	}
}















void UInventoryMainWidget::UpdateWeaponPanel()
{
	// 현재 장착 무기 및 부착물 상태를 UI에 반영
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: InventoryComp is null"));
		return;
	}

	FName ArmorID = InventoryComp->GetEquippedArmorID();
	if (!ArmorID.IsNone() && Slot_Armor)
	{
		FInventoryItem ArmorSlotItem;
		ArmorSlotItem.ItemID = ArmorID;
		ArmorSlotItem.Quantity = 1;
		Slot_Armor->InitSlot(ArmorSlotItem);
	}
	else if (Slot_Armor)
	{
		Slot_Armor->ClearSlot();
	}

	AWeaponItem* CurrentWeapon = InventoryComp->GetEquippedWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: No equipped weapon"));
		// 무기가 없으면 무기 이미지/부착 슬롯들을 숨김
		if (Img_Weapon)
		{
			Img_Weapon->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Barrel && Txt_Barrel)
		{
			Slot_Barrel->SetVisibility(ESlateVisibility::Hidden);
			Txt_Barrel->SetVisibility(ESlateVisibility::Hidden);
		}

		if (Slot_Magazine && Txt_Magazine)
		{
			Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
			Txt_Magazine->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Scope && Txt_Scope)
		{
			Slot_Scope->SetVisibility(ESlateVisibility::Hidden);
			Txt_Scope->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Underbarrel && Txt_Underbarrel)
		{
			Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
			Txt_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Stock && Txt_Stock)
		{
			Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
			Txt_Stock->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Weapon) Slot_Weapon->ClearSlot();

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: Equipped weapon=%s"), *GetNameSafe(CurrentWeapon));

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: GameInstance is null"));
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: ItemDataSubsystem is null"));
		return;
	}

	FName WeaponID = CurrentWeapon->GetItemID();
	UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: WeaponID=%s"), *WeaponID.ToString());
	FItemData WeaponData = ItemDataSubsystem->GetItemDataByID(WeaponID);
	Img_Weapon->SetBrushFromTexture(WeaponData.WeaponImage.LoadSynchronous());
	Img_Weapon->SetVisibility(ESlateVisibility::Visible);

	// 무기 타입에 따라 표시할 수 있는 부착 슬롯만 노출

	if (Slot_Weapon)
	{
		FInventoryItem WeaponSlotItem;
		WeaponSlotItem.ItemID = WeaponID;
		WeaponSlotItem.Quantity = 1;
		Slot_Weapon->InitSlot(WeaponSlotItem);
	}

	if (Slot_Scope && Txt_Scope)
	{
		Slot_Scope->SetVisibility(ESlateVisibility::Visible);
		Txt_Scope->SetVisibility(ESlateVisibility::Visible);
	}
	if (Slot_Barrel && Txt_Barrel)
	{
		Slot_Barrel->SetVisibility(ESlateVisibility::Visible);
		Txt_Barrel->SetVisibility(ESlateVisibility::Visible);
	}
	if (Slot_Magazine && Txt_Magazine)
	{
		Slot_Magazine->SetVisibility(ESlateVisibility::Visible);
		Txt_Magazine->SetVisibility(ESlateVisibility::Visible);
	}
	if (Slot_Underbarrel && Txt_Underbarrel)
	{
		Slot_Underbarrel->SetVisibility(ESlateVisibility::Visible);
		Txt_Underbarrel->SetVisibility(ESlateVisibility::Visible);
	}
	if (Slot_Stock && Txt_Stock)
	{
		Slot_Stock->SetVisibility(ESlateVisibility::Visible);
		Txt_Stock->SetVisibility(ESlateVisibility::Visible);
	}
	switch (WeaponData.WeaponType)
	{
	case EWeaponType::WT_Pistol:
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: WeaponType=Pistol"));
		if (Slot_Underbarrel && Txt_Underbarrel)
		{
			Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
			Txt_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Stock && Txt_Stock)
		{
			Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
			Txt_Stock->SetVisibility(ESlateVisibility::Hidden);
		}
		break;
	case EWeaponType::WT_Shotgun:
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: WeaponType=Shotgun"));
		if (Slot_Magazine && Txt_Magazine)
		{
			Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
			Txt_Magazine->SetVisibility(ESlateVisibility::Hidden);
		}
		break;
	case EWeaponType::WT_Melee:
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: WeaponType=Melee"));
		if (Slot_Scope && Txt_Scope)
		{
			Slot_Scope->SetVisibility(ESlateVisibility::Hidden);
			Txt_Scope->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Barrel && Txt_Barrel)
		{
			Slot_Barrel->SetVisibility(ESlateVisibility::Hidden);
			Txt_Barrel->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Magazine && Txt_Magazine)
		{
			Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
			Txt_Magazine->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Underbarrel && Txt_Underbarrel)
		{
			Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
			Txt_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Slot_Stock && Txt_Stock)
		{
			Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
			Txt_Stock->SetVisibility(ESlateVisibility::Hidden);
		}
		break;
	}

	const TMap<EAttachmentType, FName>& Attachments = CurrentWeapon->GetAttachmentState();
	UpdateAttachmentSlot(Slot_Scope, EAttachmentType::AT_Scope, Attachments);
	UpdateAttachmentSlot(Slot_Barrel, EAttachmentType::AT_Barrel, Attachments);
	UpdateAttachmentSlot(Slot_Magazine, EAttachmentType::AT_Magazine, Attachments);
	UpdateAttachmentSlot(Slot_Underbarrel, EAttachmentType::AT_Underbarrel, Attachments);
	UpdateAttachmentSlot(Slot_Stock, EAttachmentType::AT_Stock, Attachments);
	UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPanel: AttachmentStateCount=%d"), Attachments.Num());

}











void UInventoryMainWidget::HandleSlotSelection(UInventoryItemSlot* NewSlot)
{
	// 마지막 선택 슬롯을 추적하면서 선택 표시를 갱신
	if (LastSelectedSlot && LastSelectedSlot != NewSlot)
	{
		LastSelectedSlot->SetSelectedSlot(false);
	}
	if (NewSlot)
	{
		NewSlot->SetSelectedSlot(true);
		LastSelectedSlot = NewSlot;
	}
}



bool UInventoryMainWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryDragDropOperation* InventoryOp = Cast<UInventoryDragDropOperation>(InOperation);
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: Operation=%s"), *GetNameSafe(InOperation));

	// 퀵슬롯에서 드래그한 아이템을 밖(빈 공간)에 드롭하면 해당 퀵슬롯을 비움
	if (InventoryOp && InventoryOp->SourceType == static_cast<uint8>(ESlotType::ST_QuickSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: QuickSlot source index=%d"), InventoryOp->SourceIndex);
		if (InventoryComp)
		{
			InventoryComp->AssignToQuickSlot(InventoryOp->SourceIndex, NAME_None);
			RefreshInventory();
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop: InventoryComp is null; cannot clear quick slot"));
	}


	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryMainWidget::ShowTooltip(const FInventoryItem& Item)
{
	// 아이템 정보 툴팁 표시 및 내용 갱신
	UE_LOG(LogTemp, Warning, TEXT("ShowTooltip: ItemID=%s"), *Item.ItemID.ToString());
	if (ActiveTooltip)
	{
		UInventoryTooltip* Tooltip = Cast<UInventoryTooltip>(ActiveTooltip);
		if (Tooltip)
		{
			Tooltip->UpdateTooltip(Item);
			ActiveTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
			UpdateTooltipPosition();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowTooltip: Failed to cast ActiveTooltip to UInventoryTooltip"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowTooltip: ActiveTooltip is null"));
	}
}

void UInventoryMainWidget::HideTooltip()
{
	// 툴팁 숨김
	UE_LOG(LogTemp, Warning, TEXT("HideTooltip"));
	if (ActiveTooltip)
	{
		ActiveTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryMainWidget::OpenContextMenu(const FInventoryItem& Item, ESlotType SlotType, int32 SlotIndex)
{
	// 우클릭 컨텍스트 메뉴 열기 및 대상 아이템/컴포넌트 전달
	UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: ItemID=%s"), *Item.ItemID.ToString());
	if (!ContextMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: ContextMenuWidgetClass is null"));
		return;
	}

	if (!ActiveContextMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: Creating context menu widget"));
		ActiveContextMenu = CreateWidget<UInventoryActionMenu>(this, ContextMenuWidgetClass);
	}

	if (ActiveContextMenu && !ActiveContextMenu->IsInViewport())
	{
		ActiveContextMenu->AddToViewport(120);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: Failed to create context menu widget"));
	}


	if (ActiveContextMenu)
	{
		bIsContextMenuOpen = true;

		// 메뉴가 조작할 대상 설정
		ActiveContextMenu->Setup(Item, InventoryComp, SlotType, SlotIndex, this);

		ActiveContextMenu->SetVisibility(ESlateVisibility::Visible);

		if (APlayerController* PC = GetOwningPlayer())
		{
			float MouseX, MouseY;
			if (PC->GetMousePosition(MouseX, MouseY))
			{
				// 마우스 위치 기준으로 메뉴 위치 지정
				UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: Positioning at (%.1f, %.1f)"), MouseX, MouseY);
				ActiveContextMenu->SetPositionInViewport(FVector2D(MouseX, MouseY));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: Failed to get mouse position"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OpenContextMenu: Owning player controller is null"));
		}
	}
}

void UInventoryMainWidget::CloseContextMenu()
{
	// 컨텍스트 메뉴 닫기
	UE_LOG(LogTemp, Warning, TEXT("CloseContextMenu"));
	bIsContextMenuOpen = false;
	if (ActiveContextMenu)
	{
		ActiveContextMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryMainWidget::UpdateTooltipPosition()
{
	// 툴팁을 마우스 커서 근처로 이동
	APlayerController* PC = GetOwningPlayer();
	if (!PC || !ActiveTooltip)
	{
		return;
	}
	float MouseX, MouseY;
	if (PC->GetMousePosition(MouseX, MouseY))
	{
		ActiveTooltip->ForceLayoutPrepass();
		FVector2D TooltipSize = ActiveTooltip->GetDesiredSize();

		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

		FVector2D RealTooltipSize = TooltipSize * DPIScale;

		FVector2D TargetPosition(MouseX, MouseY);

		if (TargetPosition.X + RealTooltipSize.X > ViewportSize.X)
		{
			TargetPosition.X = MouseX - RealTooltipSize.X - 10;
		}
		if (TargetPosition.Y + RealTooltipSize.Y > ViewportSize.Y)
		{
			TargetPosition.Y = MouseY - RealTooltipSize.Y - 10;
		}

		ActiveTooltip->SetPositionInViewport(TargetPosition);
	}
}

void UInventoryMainWidget::UpdateAttachmentSlot(UInventoryItemSlot* AttachmentSlot, EAttachmentType Type, const TMap<EAttachmentType, FName>& Attachments)
{
	//슬롯이 없거나 보이지 않는 상태면 return
	if (!AttachmentSlot || AttachmentSlot->GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	//장착된 부착물이 있는지 확인
	if (Attachments.Contains(Type) && !Attachments[Type].IsNone())
	{
		FInventoryItem AttachmentItem;
		AttachmentItem.ItemID = Attachments[Type];
		AttachmentItem.Quantity = 1;
		AttachmentSlot->InitSlot(AttachmentItem);
	}
	else
	{
		//장착된 부착물이 없으면 슬롯 비움
		AttachmentSlot->ClearSlot();
	}
}
