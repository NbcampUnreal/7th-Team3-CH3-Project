// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"



UCLASS()
class TEAM3_PROJECT_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
	int32 SourceIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
	uint8 SourceType; // 0: 인벤토리, 1: 부착물 슬롯, 2: 퀵슬롯
};
