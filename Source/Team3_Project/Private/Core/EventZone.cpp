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

}

void AEventZone::BeginPlay()
{
	Super::BeginPlay();
	Debug::Print(FString::FromInt(Id) + TEXT("EventZone생성"), FColor::Blue);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AEventZone::OnComponentBeginOverlap);
}

void AEventZone::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	/*Debug::Print(*OverlappedComponent->GetName(),FColor::Blue);
	Debug::Print(*OtherActor->GetName(),FColor::Red);
	Debug::Print(*OtherComp->GetName(),FColor::Green);*/

	AMainGameState* CurrentGameState = AMainGameState::Get(GetWorld());

	if (CurrentGameState == nullptr)
	{
		Debug::Print(TEXT("Can not Found GameState"));
		return;
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		//@TODO_Core : 플레이어 들어왔습니다 이벤트 시작 고고
		
		Debug::Print(TEXT("Player EventZone 통과 이벤트 발생합니다!"));
		CurrentGameState->OnTriggerEvent(GetId());
	}

	//@TODO_Core : 이벤트 시작 전달 해주는 로직 구현 OtherActor == player 검사 후 true 라면 스포너 작동, false라면 무시 
}
