// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Shared/ItemTypes.h"
#include "EquipableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEquipableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEAM3_PROJECT_API IEquipableInterface
{
	GENERATED_BODY()

public:
	virtual void EquipItemByData(const FInventoryItem& ItemData, ESlotType SlotType) = 0;

	virtual FInventoryItem UnequipItemBySlot(ESlotType SlotType) const = 0;
};
