// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDef.h"
#include "InventoryComponent.generated.h"



USTRUCT(BlueprintType)
struct FItemStack
	// Represents a single inventory slot containing an item type and its quantity.
// Used so the inventory can store multiple item stacks (e.g., 3 Health Packs, 2 Flashbangs, etc.).
// so we can make an instance of the struct and use it for each item 

{
	GENERATED_BODY()
	// Required Unreal macro that generates boilerplate code for reflection,
// serialization, and Blueprint integration. It lets the engine recognize
// this struct/class and expose its properties/functions to the editor.


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UItemDef> Item = nullptr;
	// "TObjectPtr" is a type of point to a UObject thats meant to be a drop in replacement for a raw pointer
	//"Item"  is a Reference to the Data Asset defining the health pack

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0; 
	//How many of this item  you own in your inventory right now.


	bool isValid() const { return Item && Count > 0; }
	//just a quick way to check whether this inventory slot (or “stack”) actually contains something useful.
	//evaluates to true only if both of these are true:
	//Item is not nullptr (so there’s actually an item assigned)
	//Count is greater than zero(so you actually have some of it)
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
// "ClassGroup simply controls where the component appears in the add Component drop down  in unreal 
// By default, custom C++ components are not automatically visible in the Blueprint
// "Add Component" list. They can only be attached through C++ code.
//
// Adding the meta tag "BlueprintSpawnableComponent" makes this component appear
// in the Blueprint Editor's Add Component menu, allowing designers to:
//
//   • Add this component to any Actor or Pawn directly in the Blueprint Editor.
//   • View and edit its properties in the Details panel.
//   • Have it automatically created and initialized when the Blueprint spawns.
//
// This is what lets UInventoryComponent be reusabl

class UInventoryComponent : public UActorComponent
{

	GENERATED_BODY()

public: 
	UInventoryComponent(); // constructor function

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemStack> Hotbar;  // create an array to hold are different items 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "inventory")
	int32 ActiveSlotIndex = 0; // this will tell us which slot is currently equipped/ selected

	bool AddItem(UItemDef* Item, int32 Count); 
	// the fucntion that will allow us to add an item to are invtory it needs to have a reference tpo the item and how many we have to see if more can be added 


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectSlot(int32 Index);
	//This function is for changing which inventory slot is currently active (selected) — not for using or consuming the item yet.


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseActive(AActor* User);
		// Use/consume currently selected slot (triggered by Mouse/Use)

private: 
	int32 TryStackIntoExisting(UItemDef* Item, int32 Count);
	// Try to stack into existing slot with same item type. Returns remaining count.

	bool TryPlaceFirstInEmpty(UItemDef* Item, int32 Count);
		// Place into first empty slot; auto-equips that slot. Returns true if placed.


	//Utility lookups
	int32 FindSlotWithItem(UItemDef* Item) const;
	// try and find which slot has this item type

		int32 FindFirstEmptySlot() const;
	// try and find the first empty slot with no item

	bool  HasThreeUniqueTypes() const;
	//checking if we already have three unique items to determine if we can hold anythting else
};

