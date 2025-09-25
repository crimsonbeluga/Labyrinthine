// AMazeGameModeBase.cpp
#include "AMazeGameModeBase.h"
#include "AMazeCharacter.h" // your character class

AAMazeGameModeBase::AAMazeGameModeBase()
{
	// Make THIS the pawn that spawns for the player
	DefaultPawnClass = AAMazeCharacter::StaticClass();
	// (Optional) If/when you make a custom PlayerController:
	// PlayerControllerClass = AYourPlayerController::StaticClass();
}
