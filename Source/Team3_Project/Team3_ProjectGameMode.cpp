// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team3_ProjectGameMode.h"
#include "Team3_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATeam3_ProjectGameMode::ATeam3_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
