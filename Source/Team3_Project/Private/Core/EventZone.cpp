// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EventZone.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AEventZone::AEventZone()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(RootComponent);

}
