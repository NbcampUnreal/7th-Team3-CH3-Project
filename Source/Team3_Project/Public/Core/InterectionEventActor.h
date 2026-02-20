// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InterectionEventActor.generated.h"

UCLASS()
class TEAM3_PROJECT_API AInterectionEventActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AInterectionEventActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UWidgetComponent> MessageWidget;

protected:
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlapComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnComponentEndOverlap(
		UPrimitiveComponent* OverlapEndComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherIndexBody
	);
};
