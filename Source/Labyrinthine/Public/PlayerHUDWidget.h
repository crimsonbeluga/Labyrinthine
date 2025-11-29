// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;

UCLASS()
class LABYRINTHINE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	// `meta = (BindWidget)` tells Unreal’s UMG system to automatically link this C++ variable
// to a widget of the same name inside the Widget Blueprint that inherits from this class.
//
// Requirements for BindWidget to work:
//   • This C++ class must derive from UUserWidget.
//   • The Widget Blueprint must inherit from THIS C++ class.
//   • The Blueprint must contain a widget named exactly "HealthBar".
//   • That widget must be of a compatible type (UProgressBar in this case).
//
// When the widget is constructed, Unreal will automatically find the Blueprint widget
// and assign it to this pointer, allowing C++ to control the UI element directly
// without manual lookups or Blueprint variable binding.

protected:

	virtual void NativeOnInitialized() override;
	// Called when the widget is constructed and ready to bind.

		// Function called whenever the character's health changes.
	void HandleHealthChanged(float NewHealht, float MaxHealth);
	
};
