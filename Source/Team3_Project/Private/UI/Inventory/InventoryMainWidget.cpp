// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryMainWidget.h"
#include "Components/UniformGridPanel.h"
#include "Item/InventoryComponent.h"
#include "UI/Inventory/InventoryItemSlot.h"

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
}