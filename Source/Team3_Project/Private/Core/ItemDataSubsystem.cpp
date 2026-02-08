// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ItemDataSubsystem.h"

void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 아이템 데이터 테이블 로드
	FString DataTablePath = TEXT("/Game/Team3_Project/DataTables/DT_ItemData.DT_ItemData");
	ItemDataTable = Cast<UDataTable>(StaticLoadObject(
		UDataTable::StaticClass(),
		nullptr,
		*DataTablePath
	));

	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Item Data Table at path: %s"), *DataTablePath);
	}
}

FItemData UItemDataSubsystem::GetItemDataByID(FName ItemID) const
{
	// 아이템 데이터 테이블이 유효한지 확인
	if (!ItemDataTable || ItemID.IsNone())
	{
		return FItemData();
	}

	// 데이터 테이블에서 아이템 데이터 검색
	static const FString ContextString(TEXT("Item Data Context"));
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemID, ContextString);
	if (ItemData)
	{
		return *ItemData;
	}

	UE_LOG(LogTemp, Warning, TEXT("Item ID %s not found in Item Data Table."), *ItemID.ToString());
	return FItemData();
}