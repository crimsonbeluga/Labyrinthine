// AMazeCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" // Enhanced Input
#include "AMazeCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;
class UInventoryComponent;

UCLASS()
class LABYRINTHINE_API AAMazeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAMazeCharacter();

	void DealDamage(float DamageAmount);
	void HandleDeath();

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float currentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float startingHealth = 100;

	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "50", ClampMax = "5000"))
	float InteractTraceDistance = 500.f;   // how far the sweep goes

	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "0", ClampMax = "100"))
	float InteractTraceRadius = 16.f;      // how wide the sweep is

	UPROPERTY(EditAnywhere, Category = "Interaction|Trace", meta = (ClampMin = "0", ClampMax = "45"))
	float InteractMaxAimAngleDeg = 18.f;   // optional aim gating (set to 0 to disable)


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	// Assign these in your BP (child of this class)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Move;

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

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	class USphereComponent* InteractRange = nullptr; 
	// creating the pointer that will store the memory address if the sphere component that determines if were with in interact range
	// Setting "InteractRange" to nullptr is defensive programming — it avoids undefined behavior if something tries to access it before the constructor assigns it.

		// The most recent trace hit result — used to interact directly
	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	FHitResult LastInteractHit;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory = nullptr;


	void UpdateInteractionFocus();      // called from Tick
	//This function’s job is to update which object your player is currently looking at and can interact with.
	// Performs a forward line trace from the player's camera each frame to detect potential interactable objects.
    // If the trace hits a valid actor within interaction range, stores it as the current FocusedActor.
    // Otherwise clears FocusedActor to indicate nothing is in focus.
	// This function keeps track of what the player is looking at for interaction input.

	bool IsWithinInteractRange(const AActor* Target) const;
// Function that checks whether the focused actor (are interactable object) is close enough to  be interacted with.
 // Returns true if the actor is within the defined interaction range; false otherwise.
 // -------------------------------------------------------------------------------
// (const AActor* Target)→ The function parameter list. It takes one argument:
// - the first  const          → The function promises not to modify the actor passed in.
//  the last const → Function-level const qualifier. This means the function itself does not modify any member variables of this class (it’s a read-only check).         



// -----------------------------------------------------------------------------
// Input Callback Functions
// These functions are bound to Enhanced Input actions via delegates in
// SetupPlayerInputComponent(). Each one handles the corresponding gameplay input
// event (movement, looking, jumping, interacting, using items, selecting slots, etc.)
// -----------------------------------------------------------------------------
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void OnJumpPressed(const FInputActionValue& Value);
	void OnJumpReleased(const FInputActionValue& Value);
	void OnInteractPressed(const FInputActionValue& Value);
	void OnUsePressed(const FInputActionValue& Value);
	void OnItemSlotOnePressed(const FInputActionValue& Value);
	void OnItemSlotTwoPressed(const FInputActionValue& Value);
	void OnItemSlotThreePressed(const FInputActionValue& Value);
private:
	void SetupDefaultInputMapping(); 

};
