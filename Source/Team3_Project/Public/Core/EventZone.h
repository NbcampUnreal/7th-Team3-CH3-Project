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

	void InitId(int32 OwnId) { Id = OwnId; }
	int32 GetId() const { return Id; }

public:
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	
protected:
	virtual void BeginPlay() override;

private:
	//객체 고유 Id
	UPROPERTY(VisibleInstanceOnly, Category = "ID")
	int32 Id = 0;

};
