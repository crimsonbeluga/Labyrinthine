// InventoryComponent.h
// This header declares the inventory component class and the FItemStack struct used to store items.
// The component exposes a simple 3-slot hotbar with basic operations: add items, find slots, select active slot, and use the active item.

#pragma once

#include "CoreMinimal.h"                 // Core Unreal types and utilities (FString, TArray, INDEX_NONE, etc.)
#include "Components/ActorComponent.h"   // Base class for UActorComponent
#include "InventoryComponent.generated.h"// Required for UHT (Unreal Header Tool) to generate reflection code

class UItemDef; // Forward declaration: UItemDef is a data asset describing an item type (name, icon, MaxStack, etc.)

// ============================
// FItemStack: a single hotbar slot
// ============================
// Represents one stack of items in the hotbar.
// Holds a pointer to the item definition (type) and a count (quantity).
USTRUCT(BlueprintType)
struct FItemStack
{
	GENERATED_BODY() // Expands to boilerplate needed by Unreal's reflection system for USTRUCTs

		// The type of item stored in this stack (nullptr means the slot is empty or invalid).
		// Marked EditAnywhere/BlueprintReadWrite so designers can view/edit in editor and Blueprints can read/write it.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemDef* Item = nullptr;

	// How many copies of this item are in the stack. 0 means empty.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	// Convenience helper: a stack is "valid" only if it has a real item AND a positive count.
	// (Used in logic to check quickly whether a slot is occupied.)
	bool isValid() const { return Item != nullptr && Count > 0; }
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
// ==================================
// UInventoryComponent: the hotbar API
// ==================================
// Actor component that manages a fixed-size hotbar (3 slots).
// It provides functions to add items (with stacking), find existing stacks, select which slot is active,
// and consume/use the active slot. It’s designed to be simple and easily expanded later.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LABYRINTHINE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY() // Required by Unreal for reflection and component lifecycle

public:
	// Constructor: sets defaults (e.g., disables ticking, sizes hotbar to 3, sets initial active slot).
	UInventoryComponent();

	// ======================
	// Public Gameplay API
	// ======================

	// Add Count copies of Item to the inventory.
	// Returns true if any amount was successfully added (even partially),
	// false if nothing could be added (e.g., full and blocked by rules).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemDef* Item, int32 Count);

	// Find the index of a slot that already contains this Item (and has Count > 0).
	// Returns INDEX_NONE (-1) if no such slot exists.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 FindSlotWithItem(UItemDef* Item) const;

	// Change which hotbar slot is considered "active" (e.g., what the player is holding).
	// This does not validate whether the slot is empty — UI/game logic can handle that.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectSlot(int32 Index);

	// Use/consume one unit from the active slot.
	// Returns false if the active slot is invalid or empty.
	// NOTE: This is a stub for now; in a complete system, this would trigger item behavior via UItemDef.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseActive(AActor* User);
	// Event you can bind to in Blueprints (e.g. UI widget binds and calls RefreshInventory)
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;


	// ======================
	// Data (Editable/Visible)
	// ======================

	// The fixed hotbar array. In constructor we size this to 3 (indices 0..2).
	// Marked EditAnywhere so designers can inspect in editor; BlueprintReadOnly so BP can read the state.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemStack> Hotbar;

	// Which slot index is currently active/selected. Defaults to 0.
	// Note: Validity is NOT guaranteed (UI/gameplay should handle empty vs valid slots).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveSlotIndex = 0;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(const UItemDef* Item, int32 MinCount = 1) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(const UItemDef* Item, int32 Amount = 1);


private:
	// ======================
	// Internal Helpers (CPP)
	// ======================

	// Finds the first slot that is "empty" (no Item OR Count <= 0).
	// Returns INDEX_NONE if there is no available empty slot.
	int32 FindFirstEmptySlot() const;

	// Returns true if the hotbar already contains 3 or more UNIQUE item types.
	// Used to enforce the "max 3 unique types" rule before introducing a brand-new type.
	bool HasThreeUniqueTypes() const;



};
