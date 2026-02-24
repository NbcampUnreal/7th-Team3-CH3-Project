// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "DoorNPC.generated.h"

UCLASS()
class TEAM3_PROJECT_API ADoorNPC : public AActor , public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ADoorNPC();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void SetInteractFocus_Implementation(bool bIsFocus) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USceneComponent> RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> SphereComp;
};
