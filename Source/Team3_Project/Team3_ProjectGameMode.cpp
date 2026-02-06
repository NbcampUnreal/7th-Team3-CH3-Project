// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team3_ProjectGameMode.h"
#include "Team3_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATeam3_ProjectGameMode::ATeam3_ProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
