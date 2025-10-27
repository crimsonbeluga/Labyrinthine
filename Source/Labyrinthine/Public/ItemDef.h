// ItemDef.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDef.generated.h"

// Forward declarations
class UTexture2D;
class UItemUseBehavior;
class UAnimMontage;

UCLASS(BlueprintType)
class UItemDef : public UDataAsset
{
	GENERATED_BODY()

public:

	// -------------------------------------------------------------------------
	// Basic item information
	// -------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = 1))
	int32 MaxStack = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Value = 0.f;

	// -------------------------------------------------------------------------
	// Use-related properties
	// -------------------------------------------------------------------------
	// Whether the item should be consumed when successfully used
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Use")
	bool bConsumable = true;

	// The object that defines what happens when the item is used
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item|Use")
	TObjectPtr<UItemUseBehavior> UseBehavior = nullptr;

	// Optional animation montage to play when using the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Use")
	TObjectPtr<UAnimMontage> UseMontage = nullptr;
};
