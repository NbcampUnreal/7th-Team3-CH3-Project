// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"

AMainGameState::AMainGameState()
{
	
}

AMainGameState* AMainGameState::Get(const UWorld* WorldObject)
{
	UWorld* CurrentWorld = WorldObject->GetWorld();
	if (CurrentWorld == nullptr)
	{
		return nullptr;
	}

	AMainGameState* CurrentGameState = Cast<AMainGameState>(CurrentWorld->GetGameState());
	if (CurrentGameState == nullptr)
	{
		return nullptr;
	}

	return CurrentGameState;
}