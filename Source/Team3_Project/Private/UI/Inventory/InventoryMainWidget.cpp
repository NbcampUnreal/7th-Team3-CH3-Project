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

void UInventoryMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (ActiveTooltip && ActiveTooltip->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		UpdateTooltipPosition();
	}
}

FReply UInventoryMainWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsContextMenuOpen)
	{
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
	//인벤토리 업데이트 델리게이트에 바인딩
	InventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryMainWidget::RefreshInventory);

	RefreshInventory();
}

void UInventoryMainWidget::RefreshInventory()
{
	if (!InventoryComp || !InventoryGrid || !SlotWidgetClass)
	{
		return;
	}
	//기존 슬롯들 제거
	InventoryGrid->ClearChildren();
	//인벤토리 아이템 가져오기
	const TArray<FInventoryItem>& Items = InventoryComp->GetInventoryContents();

	int32 TotalSlots = InventoryComp->GetCapacity();
	//그리드에 슬롯 추가
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		//새 슬롯 위젯 생성
		UInventoryItemSlot* NewSlot = CreateWidget<UInventoryItemSlot>(this, SlotWidgetClass);
		if (NewSlot)
		{
			NewSlot->ParentInventoryWidget = this;
			//아이템이 있으면 초기화, 없으면 비우기
			if (Items.IsValidIndex(i))
			{
				NewSlot->InitSlot(Items[i]);
			}
			else
			{
				NewSlot->ClearSlot();
			}

			//행과 열 계산 후 그리드에 추가
			int32 Row = i / Columns;
			int32 Column = i % Columns;

			InventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
		}
	}
}

void UInventoryMainWidget::HandleSlotHover(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item)
{
	if (bIsContextMenuOpen)
	{
		return;
	}
	ShowTooltip(Item);
}

void UInventoryMainWidget::HandleSlotUnhover(UInventoryItemSlot* ItemSlot)
{
	HideTooltip();
}

void UInventoryMainWidget::HandleSlotRightClick(UInventoryItemSlot* ItemSlot, const FInventoryItem& Item)
{
	HideTooltip();
	OpenContextMenu(Item);
}

void UInventoryMainWidget::UpdateWeaponPanel()
{
	if (!InventoryComp)
	{
		return;
	}

	AWeaponItem* CurrentWeapon = InventoryComp->GetEquippedWeapon();

	if (!CurrentWeapon)
	{
		if (Img_Weapon)
		{
			Img_Weapon->SetVisibility(ESlateVisibility::Hidden);
		}
		Slot_Barrel->SetVisibility(ESlateVisibility::Hidden);
		Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
		Slot_Scope->SetVisibility(ESlateVisibility::Hidden);
		Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}
	
	FName WeaponID = CurrentWeapon->GetItemID();
	FItemData WeaponData = ItemDataSubsystem->GetItemDataByID(WeaponID);
	Img_Weapon->SetBrushFromTexture(WeaponData.WeaponImage.LoadSynchronous());
	Img_Weapon->SetVisibility(ESlateVisibility::Visible);

	if (Slot_Scope) Slot_Scope->SetVisibility(ESlateVisibility::Visible);
	if (Slot_Barrel) Slot_Barrel->SetVisibility(ESlateVisibility::Visible);
	if (Slot_Magazine) Slot_Magazine->SetVisibility(ESlateVisibility::Visible);
	if (Slot_Underbarrel) Slot_Underbarrel->SetVisibility(ESlateVisibility::Visible);
	if (Slot_Stock) Slot_Stock->SetVisibility(ESlateVisibility::Visible);

	switch (WeaponData.WeaponType)
	{
	case EWeaponType::WT_Pistol:
		if (Slot_Underbarrel) Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		if (Slot_Stock) Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EWeaponType::WT_Shotgun:
		if (Slot_Magazine) Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EWeaponType::WT_Melee:
		if (Slot_Scope) Slot_Scope->SetVisibility(ESlateVisibility::Hidden);
		if (Slot_Barrel) Slot_Barrel->SetVisibility(ESlateVisibility::Hidden);
		if (Slot_Magazine) Slot_Magazine->SetVisibility(ESlateVisibility::Hidden);
		if (Slot_Underbarrel) Slot_Underbarrel->SetVisibility(ESlateVisibility::Hidden);
		if (Slot_Stock) Slot_Stock->SetVisibility(ESlateVisibility::Hidden);
		break;
	}

	const TMap<EAttachmentType, FName>& Attachments = CurrentWeapon->GetAttachmentState();

	if (Slot_Scope && Slot_Scope->GetVisibility() == ESlateVisibility::Visible)
	{
		FInventoryItem ScopeItem;
		ScopeItem.ItemID = Attachments.Contains(EAttachmentType::AT_Scope) ? Attachments[EAttachmentType::AT_Scope] : FName();
		ScopeItem.Quantity = ScopeItem.ItemID.IsNone() ? 0 : 1;
		Slot_Scope->InitSlot(ScopeItem);
	}
	if (Slot_Barrel && Slot_Barrel->GetVisibility() == ESlateVisibility::Visible)
	{
		FInventoryItem BarrelItem;
		BarrelItem.ItemID = Attachments.Contains(EAttachmentType::AT_Barrel) ? Attachments[EAttachmentType::AT_Barrel] : FName();
		BarrelItem.Quantity = BarrelItem.ItemID.IsNone() ? 0 : 1;
		Slot_Barrel->InitSlot(BarrelItem);
	}
	if (Slot_Magazine && Slot_Magazine->GetVisibility() == ESlateVisibility::Visible)
	{
		FInventoryItem MagazineItem;
		MagazineItem.ItemID = Attachments.Contains(EAttachmentType::AT_Magazine) ? Attachments[EAttachmentType::AT_Magazine] : FName();
		MagazineItem.Quantity = MagazineItem.ItemID.IsNone() ? 0 : 1;
		Slot_Magazine->InitSlot(MagazineItem);
	}
	if (Slot_Underbarrel && Slot_Underbarrel->GetVisibility() == ESlateVisibility::Visible)
	{
		FInventoryItem UnderbarrelItem;
		UnderbarrelItem.ItemID = Attachments.Contains(EAttachmentType::AT_Underbarrel) ? Attachments[EAttachmentType::AT_Underbarrel] : FName();
		UnderbarrelItem.Quantity = UnderbarrelItem.ItemID.IsNone() ? 0 : 1;
		Slot_Underbarrel->InitSlot(UnderbarrelItem);
	}
	if (Slot_Stock && Slot_Stock->GetVisibility() == ESlateVisibility::Visible)
	{
		FInventoryItem StockItem;
		StockItem.ItemID = Attachments.Contains(EAttachmentType::AT_Stock) ? Attachments[EAttachmentType::AT_Stock] : FName();
		StockItem.Quantity = StockItem.ItemID.IsNone() ? 0 : 1;
		Slot_Stock->InitSlot(StockItem);
	}
}

void UInventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryComp)
	{
		return;
	}

	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (OwnerPawn)
	{
		UInventoryComponent* FoundComp = OwnerPawn->FindComponentByClass<UInventoryComponent>();
		if (FoundComp)
		{
			InitializeInventory(FoundComp);

			UE_LOG(LogTemp, Log, TEXT("InventoryComponent initialized successfully via NativeConstruct"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No InventoryComponent found on owning pawn in NativeConstruct"));
		}
	}

	if (TooltipWidgetClass)
	{
		ActiveTooltip = CreateWidget<UUserWidget>(this, TooltipWidgetClass);
		if (ActiveTooltip)
		{
			ActiveTooltip->AddToViewport(100);
			ActiveTooltip->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (ContextMenuWidgetClass)
	{
		ActiveContextMenu = CreateWidget<UUserWidget>(this, ContextMenuWidgetClass);
		if (ActiveContextMenu)
		{
			ActiveContextMenu->AddToViewport(110);
			ActiveContextMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (Slot_Scope)
	{
		Slot_Scope->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Scope);
	}
	if (Slot_Barrel)
	{
		Slot_Barrel->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Barrel);
	}
	if (Slot_Magazine)
	{
		Slot_Magazine->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Magazine);
	}
	if (Slot_Underbarrel)
	{
		Slot_Underbarrel->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Underbarrel);
	}
	if (Slot_Stock)
	{
		Slot_Stock->SetSlotType(ESlotType::ST_Attachment, (int32)EAttachmentType::AT_Stock);
	}
}

void UInventoryMainWidget::ShowTooltip(const FInventoryItem& Item)
{
	if (ActiveTooltip)
	{
		ActiveTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
		UpdateTooltipPosition();
	}
}

void UInventoryMainWidget::HideTooltip()
{
	if (ActiveTooltip)
	{
		ActiveTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryMainWidget::OpenContextMenu(const FInventoryItem& Item)
{
	if (ActiveContextMenu)
	{
		bIsContextMenuOpen = true;

		if (APlayerController* PC = GetOwningPlayer())
		{
			float MouseX, MouseY;
			if (PC->GetMousePosition(MouseX, MouseY))
			{
				ActiveContextMenu->SetPositionInViewport(FVector2D(MouseX, MouseY));
				ActiveContextMenu->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UInventoryMainWidget::CloseContextMenu()
{
	bIsContextMenuOpen = false;
	if (ActiveContextMenu)
	{
		ActiveContextMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryMainWidget::UpdateTooltipPosition()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		float MouseX, MouseY;
		if (PC->GetMousePosition(MouseX, MouseY))
		{
			ActiveTooltip->SetPositionInViewport(FVector2D(MouseX + 15.f, MouseY + 15.f));
		}
	}
}
