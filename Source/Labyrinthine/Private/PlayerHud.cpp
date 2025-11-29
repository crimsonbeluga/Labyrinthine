// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHud.h"

void APlayerHud::BeginPlay()
{
	Super::BeginPlay();

	//check if a player hud widget has been assigned in the blueprint class
	if(!PlayerHUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerHud: PlayerHUDWidgetClass is not set. Assign your 'playerHud' widget in BP_PlayerHud."));
		return;
	}

	UWorld* World = GetWorld();
	// retreive the world were currently in so the widget knows where and when it needs to show up  and store it in a pointer
	if (!World)
	{
		return;
	}

	PlayerHUDInstance = CreateWidget<UUserWidget>(World, PlayerHUDWidgetClass);
	// Use Unreal's UUserWidget factory function to create an instance of our HUD widget
// - CreateWidget<UUserWidget>(...) is a templated function that spawns a *widget instance*
//   of the specified widget class, but does not automatically add it to the viewport.
// - The template argument <UUserWidget> tells Unreal that we are creating a widget that
//   derives from UUserWidget (in this case, our Blueprint widget class `playerHud`).
// - The first parameter `World` is a pointer to the current UWorld. Unreal needs this to
//   know which game world / context the widget belongs to.
// - The second parameter `PlayerHUDWidgetClass` is a TSubclassOf<UUserWidget> that we
//   assigned in the editor to a specific Widget Blueprint (e.g. `playerHud`). This tells
//   CreateWidget *which concrete widget class* to instantiate at runtime.
// - The return value is a pointer to the newly created widget instance, so we store it in
//   `PlayerHUDInstance` to keep a reference and later call functions like AddToViewport().

	if (!PlayerHUDInstance)
	{
		UE_LOG(LogTemp,Warning,TEXT("PlayerHud: Failed to create HUD widget instance."))
		return;
	}
	// check to make sure we actually stored the widget in the pointer


	PlayerHUDInstance->AddToViewport();  
	// add the chosen hud into the view port so its visible on the players screen
}