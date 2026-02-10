// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{

}

UMainGameInstance* UMainGameInstance::Get(const UWorld* WorldObject)
{
	UWorld* CurrentWorld = WorldObject->GetWorld();
	if (CurrentWorld == nullptr)
	{
		return nullptr;
	}

	UMainGameInstance* CurrentGameInstance = Cast<UMainGameInstance>(CurrentWorld->GetGameInstance());
	if (CurrentGameInstance == nullptr)
	{
		return nullptr;
	}

	return CurrentGameInstance;
}
