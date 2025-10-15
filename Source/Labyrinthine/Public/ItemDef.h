// ItemDef.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"   // <- use this instead
#include "ItemDef.generated.h"


UCLASS(BlueprintType)
// --------------------------------------------------------------------------------------
// BlueprintType explanation:
//
// "BlueprintType" is a UCLASS specifier that tells Unreal's reflection system that this
// C++ class can be *used as a variable type* in Blueprints.
//
// In other words, marking a class as BlueprintType allows you to:
//
//   • Create variables, arrays, and properties of this class type inside Blueprints.
//   • Use this class as a parameter or return type in Blueprint functions.
//   • Assign and reference assets of this type directly from the editor.
//
// It does NOT make the class something you can inherit from in Blueprint (that would be
// "Blueprintable"), nor does it make it spawnable in the world. It simply exposes the
// type to Blueprints so that other Blueprints can reference instances or assets of it.
//
// Why we use it here:
//
//   • UItemDef is a data-only class (a PrimaryDataAsset or DataAsset) that defines item
//     properties such as name, icon, max stack, etc.
//   • We want Blueprints (like pickups, inventory widgets, or UI) to be able to hold
//     references to these ItemDef assets and read their data.
//   • Marking it as BlueprintType makes it appear as a valid type in the Blueprint
//     editor and in property dropdowns, allowing designers to assi

class UItemDef : public UDataAsset
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemId = NAME_None;
	// --------------------------------------------------------------------------------------
	// FName ItemId explanation:
	//
	// "FName" is Unreal Engine's lightweight, hashed string type used for identifiers.
	// It’s designed for fast comparisons and minimal memory usage.
	//
	// Unreal uses FName for almost all internal labels (like Actor names, property names,
	// socket names, animation states, etc.) because:
	//
	//   • FName stores each unique string once in a global name table.
	//   • Comparing two FNames is extremely fast (just compares internal indices).
	//   • It’s case-insensitive and immutable (cannot change after creation).
	//
	// Why we use FName here:
	//
	//   • "ItemId" serves as the unique logical identifier for each item type.
	//   • It’s used internally by code or inventory systems to identify item types quickly.
	//   • Unlike DisplayName (FText), ItemId is not meant for the player to see.
	//   • Designers assign each item a unique ID (e.g., "HealthPack", "Flashbang").
	//
	// The default value NAME_None represents an "empty" or uninitialized name.
	// It’s a safe default that prevents null references or invalid string access.
	//
	// Example:
	//     ItemId = "HealthPack"
	//     DisplayName = "Health Pack"
	//
	// Code might check:
	//     if (ItemDef->ItemId == "HealthPack") { HealPlayer(); }
	//
	// Summary:
	//   - FName = fast, memory-efficient ID for internal logic.
	//   - NAME_None = safe default placeholder when no ID is assigned.
	//   - Ideal for comparisons, lookups, and data-driven item referencing.
	// --------------------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;
		// --------------------------------------------------------------------------------------
// FText DisplayName explanation:
//
// "FText" is Unreal Engine's localized text type, used for any text that will be shown
// to the player on screen (UI labels, item names, dialogue, objectives, etc.).
//
// Unreal has three main text types:
//
//   • FName    → Fast, lightweight identifier (for internal use, not shown to player)
//   • FString  → Regular string (good for debug or internal data, NOT localizable)
//   • FText    → Localized display text (intended for all player-facing words)
//
// We use FText here so the item's name can support localization and appear correctly
// in UI systems that expect FText (like UMG text widgets).
//
// Why this property exists:
//
//   • "DisplayName" stores the human-readable name of the item shown in menus or HUD.
//   • It is separate from the technical ItemId (which is an FName used internally).
//   • Designers can set or localize this text directly in the editor for each item asset.
//
// Example:
//     ItemId       = "HealthPack"
//     DisplayName  = "Health Pack"
//
// Code might compare ItemId for logic, but DisplayName is what the player actually sees.
//
// Summary:
//   - FText supports translation and localization.
//   - DisplayName provides a readable, user-facing name for the item.
//   - This is the value that UI widgets or interaction prompts will display.
// --------------------------------------------------------------------------------------


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon = nullptr;
	// the visible image that will be shown in the inspector

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = 1))
	int32 MaxStack = 5;
	// how many of the item we can hold
	// The "meta" keyword lets you pass additional metadata to Unreal’s editor and Blueprint system.
	//It’s like a dictionary of key-value pairs that modify how the property behaves or looks in the editor.
	//is literally a small key–value pair where:
	//The key is ClampMin The value is 1
	// meta = ( Key = Value, Key2 = Value2, ... )
	// ClampMin restricts the property’s editable range in the Unreal Editor’s Details panel.


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Value = 0.f;
	// Generic value (e.g., heal amount). You can ignore for non-heal items.
// we might not even use this tbh just putting it here in case

};

