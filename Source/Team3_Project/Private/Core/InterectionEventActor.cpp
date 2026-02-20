// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InterectionEventActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInterectionEventActor::AInterectionEventActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	TriggerBox->SetupAttachment(RootComponent);

	MessageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	MessageWidget->SetupAttachment(RootComponent);
	MessageWidget->SetVisibility(false);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInterectionEventActor::OnComponentBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AInterectionEventActor::OnComponentEndOverlap);

	PrimaryActorTick.bCanEverTick = false;

}

void AInterectionEventActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInterectionEventActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInterectionEventActor::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlapComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		MessageWidget->SetVisibility(true);
	}
}


void AInterectionEventActor::OnComponentEndOverlap(
	UPrimitiveComponent* OverlapEndComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherIndexBody
)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		MessageWidget->SetVisibility(false);
	}
}