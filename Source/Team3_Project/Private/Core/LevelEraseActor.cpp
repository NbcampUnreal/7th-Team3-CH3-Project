// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LevelEraseActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
ALevelEraseActor::ALevelEraseActor()
{
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	PrimaryActorTick.bCanEverTick = false;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelEraseActor::OverlapBegins);
}


void ALevelEraseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelEraseActor::OverlapBegins(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	ACharacter* CurrentCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if ((OtherActor == CurrentCharacter) && LevelToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo, true);
	}
}

