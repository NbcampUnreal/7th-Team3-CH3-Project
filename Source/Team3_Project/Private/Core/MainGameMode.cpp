// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameMode.h"
#include "Core/MainGameInstance.h"
#include "Enemy/EnemyCharacter.h"

AMainGameMode::AMainGameMode()
{

}

AMainGameMode* AMainGameMode::Get(const UWorld* WorldObject)
{
	UWorld* CurrentWorld = WorldObject->GetWorld();
	if (CurrentWorld == nullptr)
	{
		return nullptr;
	}

	AMainGameMode* CurrentGameMode = Cast<AMainGameMode>(CurrentWorld->GetAuthGameMode());
	if (CurrentGameMode == nullptr)
	{
		return nullptr;
	}

	return CurrentGameMode;
}