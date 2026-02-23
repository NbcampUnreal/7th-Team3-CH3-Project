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
	// Sets default values for this actor's properties
	ACraftingStation();

	virtual void Interact(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	UStaticMeshComponent* StationMesh;

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void OpenCraftingUI(APlayerController* PC);
};
