// InventoryComponent.cpp
// This source file implements the runtime behavior for the player's inventory component.
// It manages: a fixed-size hotbar, stacking logic, adding new items, selecting slots, and consuming items.

#include "InventoryComponent.h" // Corresponding header for declarations of UInventoryComponent and FItemStack
#include "ItemDef.h"            // Defines UItemDef (the data that describes an item type: name, icon, MaxStack, etc.)
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogInventory, Log, All);

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

	UE_LOG(LogInventory, Log, TEXT("[Inventory] Constructed. HotbarSize=%d ActiveSlotIndex=%d"), Hotbar.Num(), ActiveSlotIndex);
}

// ======================================================
// FindSlotWithItem: locate an existing stack by Item type
// ======================================================
// Returns the index of a slot that contains the same UItemDef*, with Count > 0.
// If no such slot exists, returns INDEX_NONE (-1).
int32 UInventoryComponent::FindSlotWithItem(UItemDef* Item) const
{
	if (!Item)
	{
		UE_LOG(LogInventory, Warning, TEXT("[Inventory] FindSlotWithItem(NULL) -> INDEX_NONE"));
		return INDEX_NONE;
	}

	for (int32 i = 0; i < Hotbar.Num(); ++i)
	{
		if (Hotbar[i].Item == Item && Hotbar[i].Count > 0)
		{
			UE_LOG(LogInventory, Verbose, TEXT("[Inventory] FindSlotWithItem(%s) -> %d (Count=%d)"),
				*Item->GetName(), i, Hotbar[i].Count);
			return i;
		}
	}

	UE_LOG(LogInventory, Verbose, TEXT("[Inventory] FindSlotWithItem(%s) -> INDEX_NONE"), *Item->GetName());
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
		if (!Hotbar[i].Item || Hotbar[i].Count <= 0)
		{
			UE_LOG(LogInventory, Verbose, TEXT("[Inventory] FindFirstEmptySlot -> %d"), i);
			return i;
		}
	}
	UE_LOG(LogInventory, Verbose, TEXT("[Inventory] FindFirstEmptySlot -> INDEX_NONE"));
	return INDEX_NONE;
}

// ===========================================================
// HasThreeUniqueTypes: enforce "max 3 unique item types" rule
// ===========================================================
bool UInventoryComponent::HasThreeUniqueTypes() const
{
	TSet<UItemDef*> Unique;

	for (const FItemStack& S : Hotbar)
	{
		if (S.Item && S.Count > 0)
		{
			Unique.Add(S.Item);
		}
	}

	const int32 UniqueCount = Unique.Num();
	UE_LOG(LogInventory, Verbose, TEXT("[Inventory] HasThreeUniqueTypes -> %s (Unique=%d)"),
		UniqueCount >= 3 ? TEXT("TRUE") : TEXT("FALSE"), UniqueCount);

	return UniqueCount >= 3;
}

// =============================================
// AddItem: try to add N copies of a given item.
// =============================================
bool UInventoryComponent::AddItem(UItemDef* Item, int32 Count)
{
	if (!Item || Count <= 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("[Inventory] AddItem invalid args (Item=%s, Count=%d)"),
			Item ? *Item->GetName() : TEXT("NULL"), Count);
		return false;
	}

	UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem(%s, %d) BEGIN"), *Item->GetName(), Count);

	// 1) Try to stack into an existing slot of the same item
	if (int32 Slot = FindSlotWithItem(Item); Slot != INDEX_NONE)
	{
		const int32 MaxStack = FMath::Max(1, Item->MaxStack);
		const int32 FreeSpace = MaxStack - Hotbar[Slot].Count;

		if (FreeSpace <= 0)
		{
			UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Existing stack full (Slot=%d Count=%d MaxStack=%d) -> FALSE"),
				Slot, Hotbar[Slot].Count, MaxStack);
			return false;
		}

		const int32 ToAdd = FMath::Min(Count, FreeSpace);
		Hotbar[Slot].Count += ToAdd;
		UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Stacked %d into Slot=%d (Now=%d/%d)"),
			ToAdd, Slot, Hotbar[Slot].Count, MaxStack);

		int32 Remainder = Count - ToAdd;

		while (Remainder > 0)
		{
			const int32 Empty = FindFirstEmptySlot();
			if (Empty == INDEX_NONE) break;

			const int32 Chunk = FMath::Min(Remainder, MaxStack);
			Hotbar[Empty].Item = Item;
			Hotbar[Empty].Count = Chunk;
			Remainder -= Chunk;

			UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Opened new stack %d of %s in Slot=%d (Remainder=%d)"),
				Chunk, *Item->GetName(), Empty, Remainder);
		}

		UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem(%s) END -> TRUE"), *Item->GetName());
		return true;
	}

	// 2) No existing stack ? create a new stack if we can introduce a new type
	if (HasThreeUniqueTypes())
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Already at 3 unique types -> FALSE"));
		return false;
	}

	const int32 Empty = FindFirstEmptySlot();
	if (Empty == INDEX_NONE)
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: No empty slots -> FALSE"));
		return false;
	}

	const int32 MaxStack = FMath::Max(1, Item->MaxStack);
	const int32 ToAdd = FMath::Min(Count, MaxStack);

	Hotbar[Empty].Item = Item;
	Hotbar[Empty].Count = ToAdd;
	UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Seeded new stack in Slot=%d Count=%d/%d"),
		Empty, ToAdd, MaxStack);

	// Auto-select if nothing valid selected
	if (!Hotbar.IsValidIndex(ActiveSlotIndex) || !Hotbar[ActiveSlotIndex].isValid())
	{
		ActiveSlotIndex = Empty;
		UE_LOG(LogInventory, Verbose, TEXT("[Inventory] Auto-selected Slot=%d"), ActiveSlotIndex);
	}

	int32 Remainder = Count - ToAdd;
	while (Remainder > 0)
	{
		int32 NextEmpty = FindFirstEmptySlot();
		if (NextEmpty == INDEX_NONE) break;

		const int32 Chunk = FMath::Min(Remainder, MaxStack);
		Hotbar[NextEmpty].Item = Item;
		Hotbar[NextEmpty].Count = Chunk;
		Remainder -= Chunk;

		UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem: Extra stack %d of %s in Slot=%d (Remainder=%d)"),
			Chunk, *Item->GetName(), NextEmpty, Remainder);
	}

	UE_LOG(LogInventory, Log, TEXT("[Inventory] AddItem(%s) END -> TRUE"), *Item->GetName());
	return true;
}

// ==========================================
// SelectSlot: change the active hotbar index
// ==========================================
void UInventoryComponent::SelectSlot(int32 Index)
{
	if (Hotbar.IsValidIndex(Index))
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] SelectSlot(%d) (Prev=%d)"), Index, ActiveSlotIndex);
		ActiveSlotIndex = Index;
	}
	else
	{
		UE_LOG(LogInventory, Warning, TEXT("[Inventory] SelectSlot(%d) OUT OF RANGE (HotbarSize=%d)"),
			Index, Hotbar.Num());
	}
}

// =====================================
// UseActive: consume/use the active item
// =====================================
bool UInventoryComponent::UseActive(AActor* /*User*/)
{
	if (!Hotbar.IsValidIndex(ActiveSlotIndex))
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] UseActive: ActiveSlotIndex=%d INVALID"), ActiveSlotIndex);
		return false;
	}

	FItemStack& Slot = Hotbar[ActiveSlotIndex];

	if (!Slot.isValid())
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] UseActive: Slot %d is empty"), ActiveSlotIndex);
		return false;
	}

	// TODO: trigger item behavior via ItemDef.
	UE_LOG(LogInventory, Log, TEXT("[Inventory] UseActive: Using 1x %s from Slot=%d (Before=%d)"),
		Slot.Item ? *Slot.Item->GetName() : TEXT("NULL"), ActiveSlotIndex, Slot.Count);

	Slot.Count -= 1;

	if (Slot.Count <= 0)
	{
		UE_LOG(LogInventory, Log, TEXT("[Inventory] UseActive: Slot %d depleted -> clearing"), ActiveSlotIndex);
		Slot.Item = nullptr;
		Slot.Count = 0;
	}

	return true;
}

// =============================
// HasItem / ConsumeItem (debug)
// =============================
bool UInventoryComponent::HasItem(const UItemDef* Item, int32 MinCount) const
{
	if (!Item || MinCount <= 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("[Inventory] HasItem: invalid args (Item=%s, MinCount=%d)"),
			Item ? *Item->GetName() : TEXT("NULL"), MinCount);
		return false;
	}

	int32 Total = 0;
	for (const FItemStack& Slot : Hotbar)
	{
		if (Slot.Item == Item && Slot.Count > 0)
		{
			Total += Slot.Count;
			if (Total >= MinCount)
			{
				UE_LOG(LogInventory, Log, TEXT("[Inventory] HasItem(%s) -> Total=%d Needs=%d RETURN=TRUE"),
					*Item->GetName(), Total, MinCount);
				return true;
			}
		}
	}

	UE_LOG(LogInventory, Log, TEXT("[Inventory] HasItem(%s) -> Total=%d Needs=%d RETURN=FALSE"),
		*Item->GetName(), Total, MinCount);
	return false;
}

bool UInventoryComponent::ConsumeItem(const UItemDef* Item, int32 Amount)
{
	if (!Item || Amount <= 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("[Inventory] ConsumeItem: invalid args (Item=%s, Amount=%d)"),
			Item ? *Item->GetName() : TEXT("NULL"), Amount);
		return false;
	}

	int32 Remaining = Amount;

	// Consume from left to right to keep behavior predictable
	for (FItemStack& Slot : Hotbar)
	{
		if (Slot.Item != Item || Slot.Count <= 0) continue;

		const int32 Use = FMath::Min(Slot.Count, Remaining);
		UE_LOG(LogInventory, Verbose, TEXT("[Inventory] ConsumeItem: Slot pre (Item=%s Count=%d) -> Using %d"),
			*Item->GetName(), Slot.Count, Use);

		Slot.Count -= Use;
		Remaining -= Use;

		if (Slot.Count <= 0)
		{
			UE_LOG(LogInventory, Verbose, TEXT("[Inventory] ConsumeItem: Slot emptied -> clearing pointer"));
			Slot.Item = nullptr;
			Slot.Count = 0;
		}

		if (Remaining <= 0)
		{
			UE_LOG(LogInventory, Log, TEXT("[Inventory] ConsumeItem(%s, %d) -> RETURN=TRUE"),
				*Item->GetName(), Amount);
			return true;
		}
	}

	UE_LOG(LogInventory, Log, TEXT("[Inventory] ConsumeItem(%s, %d) -> RemainingAfter=%d RETURN=FALSE"),
		*Item->GetName(), Amount, Remaining);
	return false; // not enough in total
}
