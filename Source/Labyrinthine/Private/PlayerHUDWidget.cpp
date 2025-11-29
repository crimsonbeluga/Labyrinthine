// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "AMazeCharacter.h"

void UPlayerHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Get the owning pawn and cast it to your maze character
	if (AAMazeCharacter* Char = Cast<AAMazeCharacter>(GetOwningPlayerPawn()))
	{
		// Bind this widget's handler to the character's health-changed delegate
		Char->OnHealthChanged.AddDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);

		// Initialize the bar once with current values
		HandleHealthChanged(Char->currentHealth, Char->startingHealth);
	}

}

void UPlayerHUDWidget::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	if (!HealthBar || MaxHealth <= 0.0f)
	{
		return;
	}

	const float Percent = NewHealth / MaxHealth;
	HealthBar->SetPercent(Percent);
}
