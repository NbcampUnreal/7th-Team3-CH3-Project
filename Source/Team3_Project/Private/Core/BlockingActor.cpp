// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BlockingActor.h"
#include "Core/MainGameState.h"
#include "Components/BoxComponent.h"

// Sets default values
ABlockingActor::ABlockingActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(Root);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);


	PrimaryActorTick.bCanEverTick = false;
}

void ABlockingActor::EnableBlock()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABlockingActor::DisableBlock()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlockingActor::BeginPlay()
{
	Super::BeginPlay();
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	CurrentGameState->OnWaveFire.AddDynamic(this, &ABlockingActor::EnableBlock);
	CurrentGameState->OnWaveStop.AddDynamic(this, &ABlockingActor::DisableBlock);
}

