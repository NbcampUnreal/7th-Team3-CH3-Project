// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CommonDoor.h"
#include "Core/DebugHelper.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACommonDoor::ACommonDoor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	DoorMesh->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = false;
}

void ACommonDoor::Interact_Implementation(AActor* Interactor)
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

	if (bIsMoving || !DoorMesh) return;

	const bool bWantOpen = !bIsOpen;
	const FRotator OpenRot = ClosedRot + FRotator(0.f, OpenYaw, 0.f);

	StartDoorMoveTo(bWantOpen ? OpenRot : ClosedRot);

	bIsOpen = bWantOpen;
}

void ACommonDoor::SetInteractFocus_Implementation(bool bIsFocus)
{
	if (DoorMesh)
	{
		DoorMesh->SetRenderCustomDepth(bIsFocus);
		if (bIsFocus)
		{
			DoorMesh->SetCustomDepthStencilValue(1);
		}
		else
		{
			DoorMesh->SetCustomDepthStencilValue(0);
		}
	}
}

void ACommonDoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedRot = DoorMesh->GetRelativeRotation();
}

void ACommonDoor::StartDoorMoveTo(const FRotator& InTargetRot)
{
	TargetRot = InTargetRot;
	Elapsed = 0.f;
	bIsMoving = true;

	GetWorld()->GetTimerManager().SetTimer(
		DoorTimerHandle,
		this,
		&ACommonDoor::UpdateDoor,
		TimeInterval,
		true
	);
}

void ACommonDoor::UpdateDoor()
{
	Elapsed += TimeInterval;
	const float Alpha = FMath::Clamp(Elapsed / OpenDuration, 0.f, 1.f);

	const FRotator Current = DoorMesh->GetRelativeRotation();
	const FRotator Next = FMath::Lerp(Current, TargetRot, Alpha);

	DoorMesh->SetRelativeRotation(Next);

	if (Alpha >= 1.f)
	{
		DoorMesh->SetRelativeRotation(TargetRot);
		bIsMoving = false;
		GetWorld()->GetTimerManager().ClearTimer(DoorTimerHandle);
	}
}



