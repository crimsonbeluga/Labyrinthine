// ItemUseBehavior.cpp

#include "ItemUseBehavior.h"

bool UItemUseBehavior::Use_Implementation(AAMazeCharacter* User, UItemDef* ItemDef)
{
	// Default does nothing. Derived classes override this.
	return false;
}
