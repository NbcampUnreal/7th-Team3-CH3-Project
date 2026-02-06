// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/ItemTypes.h"
#include "InverntoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInverntoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM3_PROJECT_API UInverntoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInverntoryComponent();

	UFUNCTION(BlueprintCallable, Category="Inverntory")
	bool AddItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inverntory")
	bool RemoveItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItem(FName ItemID, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FInventoryItem>& GetInventoryContents() const { return InventoryContents; }

	UPROPERTY(BlueprintAssignable, Category = "Inverntory")
	FOnInverntoryUpdated OnInverntoryUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inverntory")
	TArray<FInventoryItem> InventoryContents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Capacity = 20;
		
};
