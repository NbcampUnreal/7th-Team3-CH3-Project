// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CommonDoor.generated.h"

UCLASS()
class TEAM3_PROJECT_API ACommonDoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ACommonDoor();

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void SetInteractFocus_Implementation(bool bIsFocus) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenYaw = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Door")
	float TimeInterval = 0.01f;

	FRotator ClosedRot;
	FRotator TargetRot;

	FTimerHandle DoorTimerHandle;
	float Elapsed = 0.0f;

	UFUNCTION()
	void UpdateDoor();

	void StartDoorMoveTo(const FRotator& InTargetRot);

private:
	bool bIsOpen = false;
	bool bIsMoving = false;
};
