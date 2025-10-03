// AMazePlayerController.cpp
#include "AMazePlayerController.h"
#include "AMazeCameraManager.h"

AAMazePlayerController::AAMazePlayerController()
{
    PlayerCameraManagerClass = AAMazeCameraManager::StaticClass();
}
