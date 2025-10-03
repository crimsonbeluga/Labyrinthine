// AMazeCameraManager.h
#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "AMazeCameraManager.generated.h"  // must be the LAST include

UCLASS()
class LABYRINTHINE_API AAMazeCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    AAMazeCameraManager();
};
