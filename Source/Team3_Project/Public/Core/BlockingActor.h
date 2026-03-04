// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockingActor.generated.h"

UCLASS()
class TEAM3_PROJECT_API ABlockingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockingActor();

	UFUNCTION()
	void EnableBlock();

	UFUNCTION()
	void DisableBlock();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> BoxComp;
};
