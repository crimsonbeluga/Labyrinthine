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

UCLASS()
class LABYRINTHINE_API AAMazeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAMazeCharacter();

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


	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void OnJumpPressed(const FInputActionValue& Value);
	void OnJumpReleased(const FInputActionValue& Value);

private:
	void SetupDefaultInputMapping();

};
