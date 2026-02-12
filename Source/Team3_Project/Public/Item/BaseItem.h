// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/InteractionInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class TEAM3_PROJECT_API ABaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	virtual void Interact(AActor* Interactor) override;

	virtual void BeginPlay() override;

	const FName& GetItemID() const { return ItemID; }

	const int32 GetQuantity() const { return Quantity; }

	void SetItemID(FName NewItemID) { ItemID = NewItemID; }

	void SetQuantity(int32 NewQuantity) { Quantity = NewQuantity; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

};
