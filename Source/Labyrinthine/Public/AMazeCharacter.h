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
