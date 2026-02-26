// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "DoorNPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractDoor);

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

	UFUNCTION(BlueprintCallable)
	void DestroyActor();

	UPROPERTY(BlueprintCallable)
	FOnInteractDoor OnInteractDoor;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door | Components")
	TObjectPtr<class USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door | Components")
	TObjectPtr<class UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door | UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> InteractDoorWidgetClass;

	UFUNCTION(BlueprintCallable)
	void OpenDoorInteractUI(APlayerController* PC);
	
	void BindDestroy();

private:
	bool bHasInteracted = false;
};
