#pragma once
#include "CoreMinimal.h"
#include "ItemUseBehavior.h"
#include "FlashbangUseBehavior.generated.h"

class AAMazeCharacter;
class AFlashbangActor;
class USoundBase;

UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType)
class LABYRINTHINE_API UFlashbangUseBehavior : public UItemUseBehavior
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashbang")
	TSubclassOf<AFlashbangActor> FlashbangClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashbang", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FuseSeconds = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashbang")
	float SpawnForwardOffset = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashbang|Audio")
	USoundBase* PinPullSFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashbang|Audio")
	USoundBase* DropSFX = nullptr;

	// NOTE: override the _Implementation form
	virtual bool Use_Implementation(AAMazeCharacter* User, UItemDef* ItemDef) override;
};
