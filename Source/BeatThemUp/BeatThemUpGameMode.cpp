// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatThemUpGameMode.h"
#include "BeatThemUpCharacter.h"

ABeatThemUpGameMode::ABeatThemUpGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ABeatThemUpCharacter::StaticClass();	
}
