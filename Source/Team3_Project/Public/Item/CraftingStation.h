// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/InteractionInterface.h"
#include "CraftingStation.generated.h"

class UStaticMeshComponent;

UCLASS()
class TEAM3_PROJECT_API ACraftingStation : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ACraftingStation();

	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category="Crafting")
	bool TryCraftItem(AActor* Crafter, FName RecipeID);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	UStaticMeshComponent* StationMesh;
};
