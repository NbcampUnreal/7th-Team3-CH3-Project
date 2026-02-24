// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DoorNPC.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"


ADoorNPC::ADoorNPC()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(RootComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComp->SetupAttachment(RootComp);

	PrimaryActorTick.bCanEverTick = false;
}

void ADoorNPC::BeginPlay()
{
	Super::BeginPlay();
}

void ADoorNPC::Interact_Implementation(AActor* Interactor)
{
	//Todo_@Core : 상호작용 문구출력 필요
	//Todo_@Core : NPC대화창 UI 열어야함
	//Todo_@Core : 
}

void ADoorNPC::SetInteractFocus_Implementation(bool bIsFocus)
{
}


