// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Hotbar.SetNum(3);
	ActiveSlotIndex = 0;  

	// at the start of the game set the hot bar amount we can hold to three
	// and set the active slotindex to zero cause we do not have anything. 


	
}


int32 UInventoryComponent::FindSlotWithItem(UItemDef* Item) const
// the function will return and integer (the index of the slot that contains the item).
{
	if (!Item) return -1; // checks if there was no item passed in
	//We use -1 as a sentinel value meaning “not found” or “invalid.”
	// So this line prevents crashes by exiting early if the input is bad.

	for (int32 i = 0; i <Hotbar.Num(); ++i)
		// for every integre number in are hotbar array of invetory slots until the last hot bar integer do the code below
		if (Hotbar[i].Item == Item && Hotbar[i].Count > 0)
			// if the hotbar integer (invetory slot) were currently on is storing the same type of item were trying to add and there is still any of that item left in that slot
			// then retur this current integer (this cuurrent item slot) as a match to the slot we can put it in.
		{
			return i;
		}

	return -1; // other wise return negative one meaning there is no item slot matching the type of item we trying to add into a slot


}


