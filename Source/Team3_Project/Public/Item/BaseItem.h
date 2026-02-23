// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/InteractionInterface.h"
#include "Components/WidgetComponent.h"
#include "BaseItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS(Blueprintable)
class TEAM3_PROJECT_API ABaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(AActor* Interactor) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	const FName& GetItemID() const { return ItemID; }

	const int32 GetQuantity() const { return Quantity; }

	void SetItemID(FName NewItemID) { ItemID = NewItemID; }

	void SetQuantity(int32 NewQuantity) { Quantity = NewQuantity; }

	virtual FText GetInteractionPrompt_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UWidgetComponent* LootWidget;


	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemFocus(bool bIsFocus);

	void UpdateLootWidgetTransform();
};
