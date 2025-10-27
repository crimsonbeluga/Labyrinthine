// ItemUseBehavior.h
// Base class for defining how an item is used by the player.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemUseBehavior.generated.h"

class AAMazeCharacter;
class UItemDef; // from ItemDef.h

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, BlueprintType)
class UItemUseBehavior : public UObject
{
	GENERATED_BODY()

public:
	// Return true if the item successfully performed its action and should be consumed.
	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	bool Use(AAMazeCharacter* User, UItemDef* ItemDef);
};
