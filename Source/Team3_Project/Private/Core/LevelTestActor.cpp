// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LevelTestActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
ALevelTestActor::ALevelTestActor()
{
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	PrimaryActorTick.bCanEverTick = false;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelTestActor::OverlapBegins);

}

// Called when the game starts or when spawned
void ALevelTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}



void ALevelTestActor::OverlapBegins(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	ACharacter* CurrentCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if ((OtherActor == CurrentCharacter) && LevelToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
	}
}

