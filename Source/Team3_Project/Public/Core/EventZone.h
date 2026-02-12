// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventZone.generated.h"

UCLASS()
class TEAM3_PROJECT_API AEventZone : public AActor
{
	GENERATED_BODY()

public:
	AEventZone();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TObjectPtr<class USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BoxCollision")
	TObjectPtr<class UBoxComponent> BoxComp;

};
