// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTestActor.generated.h"

UCLASS()
class TEAM3_PROJECT_API ALevelTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelTestActor();

protected:
	virtual void BeginPlay() override;


protected:

	UFUNCTION()
	void OverlapBegins(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBoxComponent> OverlapVolume;

};
