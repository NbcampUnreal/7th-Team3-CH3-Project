// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Shared/ItemTypes.h"
#include "ItemDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API UItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	FItemData GetItemDataByID(FName ItemID) const;

protected:
	UPROPERTY()
	UDataTable* ItemDataTable;

	UPROPERTY()
	UDataTable* RecipeDataTable;
};
