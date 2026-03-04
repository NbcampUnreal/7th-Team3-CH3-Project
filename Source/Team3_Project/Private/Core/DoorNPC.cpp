// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DoorNPC.h"
#include "Core/MainGameState.h"
#include "Core/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"


ADoorNPC::ADoorNPC()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(RootComp);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComp);

	PrimaryActorTick.bCanEverTick = false;
}

void ADoorNPC::BeginPlay()
{
	Super::BeginPlay();
	BindDestroy();
}

void ADoorNPC::Interact_Implementation(AActor* Interactor)
{
	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (!PlayerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if(!PC)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(this, true);
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	OpenDoorInteractUI(PC);

	if (bHasInteracted) return;

	bHasInteracted = true;
	OnInteractDoor.Broadcast();


}

void ADoorNPC::SetInteractFocus_Implementation(bool bIsFocus)
{
	if (StaticMesh)
	{
		StaticMesh->SetRenderCustomDepth(bIsFocus);
		if (bIsFocus)
		{
			StaticMesh->SetCustomDepthStencilValue(1);
		}
		else
		{
			StaticMesh->SetCustomDepthStencilValue(0);
		}
	}
}

void ADoorNPC::DestroyActor()
{
	Debug::Print(TEXT("DestroyActor 호출"));
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());
	if (CurrentGameState == nullptr) return;
	CurrentGameState->OnWaveFire.RemoveDynamic(this, &ADoorNPC::DestroyActor);
	this->Destroy();	
}

void ADoorNPC::OpenDoorInteractUI(APlayerController* PC)
{
	if (!InteractDoorWidgetClass || !PC)
	{
		return;
	}

	UUserWidget* DoorNpcWidget = CreateWidget<UUserWidget>(PC, InteractDoorWidgetClass);
	if (DoorNpcWidget)
	{
		DoorNpcWidget->AddToViewport();
	}
}

void ADoorNPC::BindDestroy()
{
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());
	if (CurrentGameState == nullptr) return;

	Debug::Print(TEXT("BindDestroy 바인딩"));

	CurrentGameState->OnWaveFire.AddDynamic(this, &ADoorNPC::DestroyActor);
}


