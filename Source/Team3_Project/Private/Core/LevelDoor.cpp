// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LevelDoor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelDoor::ALevelDoor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	LevelActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	LevelActorMesh->SetupAttachment(Root);
	
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelDoor::Interact_Implementation(AActor* Interactor)
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

	OpenNextLevelUI(PC);

}

void ALevelDoor::SetInteractFocus_Implementation(bool bIsFocus)
{
	if (LevelActorMesh)
	{
		LevelActorMesh->SetRenderCustomDepth(bIsFocus);
		if (bIsFocus)
		{
			LevelActorMesh->SetCustomDepthStencilValue(1);
		}
		else
		{
			LevelActorMesh->SetCustomDepthStencilValue(0);
		}
	}
}

void ALevelDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelDoor::OpenNextLevelUI(APlayerController* PC)
{
	if (!NextLevelWidgetClass || !PC)
	{
		return;
	}

	UUserWidget* NextLevelWidget = CreateWidget<UUserWidget>(PC, NextLevelWidgetClass);
	if (NextLevelWidget)
	{
		NextLevelWidget->AddToViewport();
	}
}

