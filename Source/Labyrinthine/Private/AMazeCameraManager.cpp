// AMazeCameraManager.cpp
#include "AMazeCameraManager.h"

AAMazeCameraManager::AAMazeCameraManager()
{
    // Negative = look UP, Positive = look DOWN
    ViewPitchMin = -90.f;
    ViewPitchMax = 90.f;
}
