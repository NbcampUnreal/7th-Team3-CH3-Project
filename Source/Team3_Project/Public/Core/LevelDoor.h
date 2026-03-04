// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "LevelDoor.generated.h"

UCLASS()
class TEAM3_PROJECT_API ALevelDoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ALevelDoor();

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void SetInteractFocus_Implementation(bool bIsFocus) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> LevelActorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door | UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> NextLevelWidgetClass;

		
	void OpenNextLevelUI(APlayerController* PC);
};
