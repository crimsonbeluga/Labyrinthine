// AMazeCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "AMazeCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;
class UInventoryComponent;

//this a delegate that fires whenever this characters health changes
// it will pass new health and max health to any listeners
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
// DECLARE -> This macro defines a new delegate *type* so it can be used elsewhere (like a function signature for events)

// DYNAMIC -> Makes the delegate visible to Unreal’s reflection system, allowing Blueprint binding and runtime binding

// MULTICAST -> Allows multiple listeners to subscribe to this event (UI, audio manager, gameplay systems, etc.)

// TwoParams -> Specifies that this delegate takes exactly two parameters in its signature

// Declares a Blueprint-visible multicast event type that takes exactly two parameters
// Name of the delegate type being defined
// Type of the first parameter
// Name of the first parameter (current health after damage/heal)
// Type of the second parameter
// Name of the second parameter (maximum health value)




UCLASS()
class LABYRINTHINE_API AAMazeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAMazeCharacter();

	void DealDamage(float DamageAmount);
	void HandleDeath();
	void AddHealth(float AddedHealthAmount);

	// AMazeCharacter.h

	UFUNCTION( Category = "Walking")
	void SetWalkSpeed();

	UFUNCTION( Category = "Running")
	void SetSprintSpeed();

UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
void ShowInteractionPrompt(const FText& PromptText);

UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
void HideInteractionPrompt();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float sprintTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float sprintRecoveryTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float sprintSpeed = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float timeSprinted;

	UPROPERTY(BlueprintReadWrite, Category = "Walking")
	float walkSpeed = 600.f;


	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float currentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float startingHealth = 100;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;
// FOnHealthChanged → The *delegate type* (created earlier with DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams).
// OnHealthChanged → The *instance* of that delegate type stored on this character.
//                    This instance holds all bound listeners and is what you call Broadcast() on.

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const
	{
		return (startingHealth > 0.0f) ? currentHealth / startingHealth : 0.0f;

	}
	// a function that will return the percent the health bar should be at this does not alter the health bar just gets the value that will be used to alter it



	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "50", ClampMax = "5000"))
	float InteractTraceDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "0", ClampMax = "100"))
	float InteractTraceRadius = 16.f;

	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "0", ClampMax = "45"))
	float InteractMaxAimAngleDeg = 18.f;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void TryUseSelectedItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	UFUNCTION(BlueprintImplementableEvent)
	void PlayJumpSound();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Pause;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Interact;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ItemSlot_1;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ItemSlot_2;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_ItemSlot_3;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Use;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Sprint;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Crouch;

	UPROPERTY(BlueprintReadOnly, Category = "Running", meta = (AllowPrivateAccess = "true"))
	FTimerHandle sprintTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Running", meta = (AllowPrivateAccess = "true"))
	FTimerHandle sprintRecoveryTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	USphereComponent* InteractRange = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	FHitResult LastInteractHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory = nullptr;


	void UpdateInteractionFocus();

	bool IsWithinInteractRange(const AActor* Target) const;

	UPROPERTY(BlueprintReadOnly)
	bool bCanSprint = true;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWalking = true;


	// Input callbacks
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void OnJumpPressed(const FInputActionValue& Value);
	void OnJumpReleased(const FInputActionValue& Value);
	void OnInteractPressed(const FInputActionValue& Value);
	void OnUsePressed(const FInputActionValue& Value);
	void OnItemSlotOnePressed(const FInputActionValue& Value);
	void OnItemSlotTwoPressed(const FInputActionValue& Value);
	void OnItemSlotThreePressed(const FInputActionValue& Value);
	void DisableSprintiing();
	void EnableSprint();
	void SprintRecoveryTimeMath();
	void OnCrouchPressed();
	void OnCrouchReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void OnPausePressed(const FInputActionValue& Value);

private:
	void SetupDefaultInputMapping();
};
