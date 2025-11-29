// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHINE_API APlayerHud : public AHUD
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	// this stores the reference to our widget that is are player hud
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;
	// Holds a reference to a Widget Blueprint class (such as 'playerHud') that inherits from UUserWidget.
// We use TSubclassOf so we can assign a Blueprint widget *class* in the editor, and then create an
// instance of that widget at runtime inside BeginPlay().


UPROPERTY()
UUserWidget* PlayerHUDInstance = nullptr;
// Instance of the HUD widget created at runtime

};
