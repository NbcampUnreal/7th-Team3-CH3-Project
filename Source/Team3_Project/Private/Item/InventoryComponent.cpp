// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	/* 
	반환값에 대하여
	0 : 모든 아이템이 성공적으로 추가됨 (true)
	0 < n < Quantity : 추가하지 못한 아이템의 수량(이 수량으로 바닥의 아이템 수를 업데이트)
	Quantity : 아무것도 추가하지 못함	 (false)
	*/

	// 입력 유효성 검사
	if (Quantity <= 0 || ItemID.IsNone())
	{
		return Quantity;
	}
	// 게임 인스턴스에서 아이템 데이터 서브시스템 가져오기
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return Quantity;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return Quantity;
	}

	// 아이템 데이터 가져오기
	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);

	// 최대 스택 크기 확인
	int32 MaxStack = ItemData.MaxStack;
	if (MaxStack <= 0) MaxStack = 1;

	int32 RemainingQuantity = Quantity;

	// 스택이 가능한 아이템인 경우 기존 스택에 추가
	if (MaxStack > 1)
	{
		for (FInventoryItem& InventoryItem : InventoryContents)
		{
			if (InventoryItem.ItemID == ItemID)
			{
				int32 AvailableSpace = MaxStack - InventoryItem.Quantity;
				if (AvailableSpace > 0)
				{
					int32 ToAdd = FMath::Min(AvailableSpace, RemainingQuantity);
					InventoryItem.Quantity += ToAdd;
					RemainingQuantity -= ToAdd;
					if (RemainingQuantity <= 0)
					{
						OnInventoryUpdated.Broadcast();
						return 0;
					}
				}
			}
		}
	}

	// 남은 수량을 새로운 스택으로 추가
	while (RemainingQuantity > 0)
	{
		if (InventoryContents.Num() >= Capacity)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
			break;
		}
		int32 ToAdd = FMath::Min(MaxStack, RemainingQuantity);

		FInventoryItem NewItem;
		NewItem.ItemID = ItemID;
		NewItem.Quantity = ToAdd;

		InventoryContents.Add(NewItem);
		RemainingQuantity -= ToAdd;
	}

	// 인벤토리가 변경되었음을 알림
	if (RemainingQuantity < Quantity)
	{
		OnInventoryUpdated.Broadcast();
	}

	return RemainingQuantity;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	// 입력 유효성 검사
	if (!HasItem(ItemID, Quantity))
	{
		return false;
	}	

	int32 RemainingToRemove = Quantity;
	// 인벤토리에서 해당 아이템 제거
	// 뒤에서부터 제거하여 인덱스 문제 방지
	for (int32 i = InventoryContents.Num() - 1; i >= 0 && RemainingToRemove > 0; --i)
	{
		if (InventoryContents[i].ItemID == ItemID)
		{
			int32 AmountInSlot = InventoryContents[i].Quantity;
			int32 AmountTaken = FMath::Min(AmountInSlot, RemainingToRemove);

			InventoryContents[i].Quantity -= AmountTaken;
			RemainingToRemove -= AmountTaken;

			if (InventoryContents[i].Quantity <= 0)
			{
				InventoryContents.RemoveAt(i);
			}

			if (RemainingToRemove <= 0)
			{
				break;
			}
		}
	}

	// 인벤토리가 변경되었음을 알림
	OnInventoryUpdated.Broadcast();

	return true;
}

// 조합 시스템 등에서 아이템 존재 여부와 수량을 확인할 때 사용
bool UInventoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
	
	// 입력 유효성 검사
	if (ItemID.IsNone() || Quantity <= 0)
	{
		return false;
	}

	int32 TotalQuantity = 0;
	// 인벤토리에서 해당 아이템의 총 수량 계산
	for (const FInventoryItem& InventoryItem : InventoryContents)
	{
		if (InventoryItem.ItemID == ItemID)
		{
			TotalQuantity += InventoryItem.Quantity;
		}
	}

	// 해당 아이템의 총 수량이 필요한 수량 이상인지 확인
	return TotalQuantity >= Quantity;
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	// 입력 유효성 검사
	if (ItemID.IsNone())
	{
		return 0;
	}
	int32 TotalQuantity = 0;

	// 인벤토리에서 해당 아이템의 총 수량 계산
	for (const FInventoryItem& InventoryItem : InventoryContents)
	{
		if (InventoryItem.ItemID == ItemID)
		{
			TotalQuantity += InventoryItem.Quantity;
		}
	}
	return TotalQuantity;
}