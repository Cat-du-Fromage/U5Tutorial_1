// Copyright Epic Games, Inc. All Rights Reserved.

#include "U5Tutorial_1GameMode.h"
#include "U5Tutorial_1Character.h"
#include "UObject/ConstructorHelpers.h"

AU5Tutorial_1GameMode::AU5Tutorial_1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
