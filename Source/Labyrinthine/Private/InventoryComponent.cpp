// InventoryComponent.cpp
// This source file implements the runtime behavior for the player's inventory component.
// It manages: a fixed-size hotbar, stacking logic, adding new items, selecting slots, and consuming items.

#include "InventoryComponent.h" // Corresponding header for declarations of UInventoryComponent and FItemStack
#include "ItemDef.h"            // Defines UItemDef (the data that describes an item type: name, icon, MaxStack, etc.)

// ===============================
// Constructor: UInventoryComponent
// ===============================
UInventoryComponent::UInventoryComponent()
{
	// This component does NOT need to Tick every frame, so we disable ticking for performance.
	// Inventory operations are event-driven (e.g., on pickup), not per-frame.
	PrimaryComponentTick.bCanEverTick = false;

	// Fixed 3-slot hotbar.
	// Allocate exactly three FItemStack entries in the Hotbar array (indices 0,1,2).
	Hotbar.SetNum(3);

	// Initialize the currently selected hotbar slot index.
	// 0 means the first slot is "active" by default (even if empty).
	ActiveSlotIndex = 0;
}

// ======================================================
// FindSlotWithItem: locate an existing stack by Item type
// ======================================================
// Returns the index of a slot that contains the same UItemDef*, with Count > 0.
// If no such slot exists, returns INDEX_NONE (-1).
int32 UInventoryComponent::FindSlotWithItem(UItemDef* Item) const
{
	// Defensive guard: if no item pointer was provided, return "not found".
	if (!Item) return INDEX_NONE;

	// Iterate over each hotbar slot by index.
	for (int32 i = 0; i < Hotbar.Num(); ++i)
	{
		// Check if this slot holds the SAME item type pointer AND has at least one in the stack.
		if (Hotbar[i].Item == Item && Hotbar[i].Count > 0)
		{
			// Found a matching stack; return its index.
			return i;
		}
	}

	// No slot matched the item type; signal "not found".
	return INDEX_NONE;
}

// ======================================================
// FindFirstEmptySlot: find the earliest available hotbar slot
// ======================================================
// A slot is considered "empty" if it has no Item assigned OR Count <= 0.
// Returns the first index that qualifies, or INDEX_NONE if none are empty.
int32 UInventoryComponent::FindFirstEmptySlot() const
{
	for (int32 i = 0; i < Hotbar.Num(); ++i)
	{
		// Empty if it either lacks an Item pointer OR has zero-or-negative count.
		if (!Hotbar[i].Item || Hotbar[i].Count <= 0)
			return i;
	}
	return INDEX_NONE;
}

// ===========================================================
// HasThreeUniqueTypes: enforce "max 3 unique item types" rule
// ===========================================================
// Builds a TSet of unique UItemDef* seen across non-empty slots.
// If the set size is >= 3, we already have 3 or more unique types.
bool UInventoryComponent::HasThreeUniqueTypes() const
{
	TSet<UItemDef*> Unique; // Set automatically ignores duplicates.

	// Range-based for: examines each stack in the hotbar.
	for (const FItemStack& S : Hotbar)
	{
		// Only count valid stacks (have an Item assigned and a positive Count).
		if (S.Item && S.Count > 0)
		{
			// Insert the item type into the set (duplicates are ignored).
			Unique.Add(S.Item);
		}
	}

	// If we already have 3 or more distinct types, return true.
	return Unique.Num() >= 3;
}

// =============================================
// AddItem: try to add N copies of a given item.
// =============================================
// Behavior:
// 1) If an existing stack of the same type is found, try to add to that stack up to MaxStack,
//    then (optionally) spill remainder into empty slots as new stacks of the SAME item.
// 2) If no existing stack is found, and we are allowed to add a NEW item type,
//    place a new stack into the first empty slot (respect MaxStack), then spill remainder into more empties.
// Returns true if any amount was successfully added; false otherwise.
bool UInventoryComponent::AddItem(UItemDef* Item, int32 Count)
{
	// Validate input: cannot add a null item or a non-positive quantity.
	if (!Item || Count <= 0) return false;

	// 1) Try to stack into an existing slot of the same item
	// Use C++17 if-with-initializer to capture the found slot index inline.
	if (int32 Slot = FindSlotWithItem(Item); Slot != INDEX_NONE)
	{
		// MaxStack is the per-item maximum stack size, but we ensure a minimum of 1 for safety.
		const int32 MaxStack = FMath::Max(1, Item->MaxStack);

		// FreeSpace is how many more of this item we can place in the found stack.
		const int32 FreeSpace = MaxStack - Hotbar[Slot].Count;

		// If the stack is already full, we cannot add to THIS stack.
		// (We still might be able to spill to empty slots, see below after ToAdd.)
		if (FreeSpace <= 0) return false;

		// ToAdd is the number we can safely add to this stack without exceeding MaxStack.
		// It is the min of the number we want to add and the stack's free space.
		const int32 ToAdd = FMath::Min(Count, FreeSpace);

		// Apply the addition to the existing stack.
		Hotbar[Slot].Count += ToAdd;

		// overflow: optionally open new stacks in empty slots
		// After filling the existing stack as much as possible,
		// compute how many items still remain to be added.
		int32 Remainder = Count - ToAdd;

		// While we still have items to place, keep looking for empty slots
		// and create new stacks of the SAME item type (this does NOT increase unique types).
		while (Remainder > 0)
		{
			const int32 Empty = FindFirstEmptySlot();
			// If there are no empty slots left, abort the loop (we added as much as we could).
			if (Empty == INDEX_NONE) break;

			// Each new stack can hold up to MaxStack.
			const int32 Chunk = FMath::Min(Remainder, MaxStack);

			// Initialize the new stack with this item type and the chunk amount.
			Hotbar[Empty].Item = Item;
			Hotbar[Empty].Count = Chunk;

			// Decrease the remainder by however many we just placed.
			Remainder -= Chunk;
		}

		// We consider this a success if we added anything at all (which we did here).
		return true;
	}

	// 2) No existing stack ? create a new stack if we can introduce a new type
	// Enforce the "3 unique item types" policy: if already at 3, we cannot add a NEW type.
	if (HasThreeUniqueTypes()) return false;

	// Find an empty slot to seed the first stack for this new item type.
	const int32 Empty = FindFirstEmptySlot();
	if (Empty == INDEX_NONE) return false; // No space available at all.

	// Respect per-item MaxStack (enforce minimum 1 for safety).
	const int32 MaxStack = FMath::Max(1, Item->MaxStack);

	// ToAdd is how many we can put into the first new stack.
	const int32 ToAdd = FMath::Min(Count, MaxStack);

	// Create the new stack in the empty slot.
	Hotbar[Empty].Item = Item;
	Hotbar[Empty].Count = ToAdd;

	// Auto-select if nothing valid selected
	// If the current ActiveSlotIndex is out of bounds OR points to an invalid/empty stack,
	// automatically select the slot we just filled so the player "holds" the new item by default.
	if (!Hotbar.IsValidIndex(ActiveSlotIndex) || !Hotbar[ActiveSlotIndex].isValid())
	{
		ActiveSlotIndex = Empty;
	}

	// overflow into other empty slots if available (same item type, doesn’t increase unique types)
	// If there are still more items left after filling the first stack,
	// keep placing additional stacks into other empty slots, also respecting MaxStack.
	int32 Remainder = Count - ToAdd;
	while (Remainder > 0)
	{
		int32 NextEmpty = FindFirstEmptySlot();
		// If we run out of empty slots, stop trying to place remainder.
		if (NextEmpty == INDEX_NONE) break;

		const int32 Chunk = FMath::Min(Remainder, MaxStack);
		Hotbar[NextEmpty].Item = Item;
		Hotbar[NextEmpty].Count = Chunk;
		Remainder -= Chunk;
	}

	// We successfully placed at least the first stack (and possibly more).
	return true;
}

// ==========================================
// SelectSlot: change the active hotbar index
// ==========================================
// Only updates if the provided index is within bounds.
// (No validation of whether the slot is empty; the UI/gameplay can decide that behavior.)
void UInventoryComponent::SelectSlot(int32 Index)
{
	if (Hotbar.IsValidIndex(Index))
	{
		ActiveSlotIndex = Index;
	}
}

// =====================================
// UseActive: consume/use the active item
// =====================================
// Returns false if the active index is invalid OR the slot is empty.
// Otherwise decrements Count by 1; if Count drops to 0, clears the slot.
// NOTE: This is a placeholder; in a full system you'd also trigger the item's effect here.
bool UInventoryComponent::UseActive(AActor* /*User*/)
{
	// Validate the active index is in range.
	if (!Hotbar.IsValidIndex(ActiveSlotIndex)) return false;

	// Get a reference to the active slot for modification.
	FItemStack& Slot = Hotbar[ActiveSlotIndex];

	// Ensure this slot actually holds an item with a positive count.
	if (!Slot.isValid()) return false;

	// TODO: trigger item behavior via ItemDef.
	// For now, we simply consume one unit of the item.
	Slot.Count -= 1;

	// If the count reaches zero or below, clear the slot to "empty" state.
	if (Slot.Count <= 0)
	{
		Slot.Item = nullptr;
		Slot.Count = 0;
	}

	return true;
}
