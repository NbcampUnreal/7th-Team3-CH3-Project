// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EventZone.h"
#include "Core/DebugHelper.h"
#include "Core/MainGameState.h"
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

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AEventZone::OnComponentBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AEventZone::OnComponentEndOverlap);

}

void AEventZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEventZone::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	if (CurrentGameState == nullptr)
	{
		return;
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		//@TODO_Core : 플레이어 들어왔습니다 이벤트 시작 고고
		
		CurrentGameState->OnTriggerEvent(GetId(), true);
	}

	//@TODO_Core : 이벤트 시작 전달 해주는 로직 구현 OtherActor == player 검사 후 true 라면 스포너 작동, false라면 무시		
}

void AEventZone::OnComponentEndOverlap(
	UPrimitiveComponent* EndOverlapComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	if (CurrentGameState == nullptr)
	{
		return;
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		CurrentGameState->OnTriggerEvent(GetId(), false);
	}
}
