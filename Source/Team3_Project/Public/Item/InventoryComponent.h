// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/ItemTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM3_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItem(FName ItemID, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FInventoryItem>& GetInventoryContents() const { return InventoryContents; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventoryItem> InventoryContents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Capacity = 20;
};
