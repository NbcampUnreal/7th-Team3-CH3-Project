// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CraftingStation.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACraftingStation::ACraftingStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StationMesh"));
	RootComponent = StationMesh;

}

void ACraftingStation::Interact(AActor* Interactor)
{
	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (!PlayerPawn)
	{
		return;
	}
	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PC)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(this, true);
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	OpenCraftingUI(PC);
}	

void ACraftingStation::OpenCraftingUI(APlayerController* PC)
{

}


