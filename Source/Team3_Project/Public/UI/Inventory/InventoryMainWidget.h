// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMainWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventoryItemSlot;

UCLASS()
class TEAM3_PROJECT_API UInventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void InitializeInventory(UInventoryComponent* NewInventoryComp);

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void RefreshInventory();

protected:
	void NativeConstruct() override;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
	TSubclassOf<UInventoryItemSlot> SlotWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory UI")
	int32 Columns = 5;

};
