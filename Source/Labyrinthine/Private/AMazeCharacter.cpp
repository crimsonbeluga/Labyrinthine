// AMazeCharacter.cpp
#include "AMazeCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Enhanced Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

// Sets default values
AAMazeCharacter::AAMazeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent); // This is valid now because of BasePawn
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

}

void AAMazeCharacter::BeginPlay()
{
    Super::BeginPlay();

    SetupDefaultInputMapping(); // grabbing the input map and setting it as the one we will be using

    currentHealth = startingHealth;
}

void AAMazeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAMazeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if(IA_Move) //if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAMazeCharacter::HandleMove);
            EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AAMazeCharacter::HandleMove); // zero on release
        }
        if (IA_Look)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AAMazeCharacter::HandleLook);
        }
        if (IA_Jump)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnJumpPressed);

            EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AAMazeCharacter::OnJumpReleased);
        }

        if (IA_Interact)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnInteractPressed);
        }

        if (IA_Use)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Use, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnUsePressed);
        }

        if (IA_ItemSlot_1)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_1, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnItemSlotOnePressed);
        }

        if (IA_ItemSlot_2)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_2, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnItemSlotTwoPressed);
        }

        if (IA_ItemSlot_3)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_3, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnItemSlotThreePressed);
        }

      
    }
}

void AAMazeCharacter::HandleMove(const FInputActionValue& Value)
// takes the input action value returned from bind action and sets it to the string name of value.
{
    const FVector2D Input = Value.Get<FVector2D>(); 
    // takes the input action that has now been renamed to "Value", retrieves the FVector2D that is stored inside that input action value.
    // then sets it equal to a constant FVector2D called input. const means constant and promises not to change the value of the variable just read it 


    if (!Controller || Input.IsNearlyZero()) return; 
    // If there is no valid controller curentlt possesing the pawn, or the input vector from our movement keys is basically zero (no movement pressed),
// then end the function and dont run the rest of the code.


    const FRotator ControlRot = Controller->GetControlRotation();
    // Gets the controller's world-space rotation (usually the camera facing direction),
    // not the character's own rotation. Used to align movement input with where the player is looking.

    const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
    // create a FRotator called "YawRot" which is a 3 point rotation (pitch, yaw, roll) then use the controller rotations  yaw (left and right) to set the YawRots yaw. 

    const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    // Using the yaw rotation, we build an FRotationMatrix. From only knowing yaw (left/right),
// the matrix can deduce all basis directions: forward/back, left/right, up/down from only knowing what left and right is. think aboout this in real life.
//     if we know which way left and right is so look to your left and look to your right. then we also know which way forward,backward,up and down is.
// Once the rotation matrix is created, we call .GetUnitAxis() to retrieve one of those unit vectors so forward backward, left riight or   up down
// Because in Unreal the X axis represents forward/backward, we request EAxis::X to get the forward vector.


    const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    // Using the yaw rotation, we build an FRotationMatrix. From only knowing yaw (left/right),
// the matrix can deduce all basis directions: forward/back, left/right, up/down from only knowing what left and right is. think aboout this in real life.
//     if we know which way left and right is so look to your left and look to your right. then we also know which way forward,backward,up and down is.
// Once the rotation matrix is created, we call .GetUnitAxis() to retrieve one of those unit vectors so forward backward, left riight or   up down
// Because in Unreal the Y axis represents Left and Right, we request EAxis::to get the Right vector.

    AddMovementInput(Forward, Input.X);
    AddMovementInput(Right, Input.Y);
}

void AAMazeCharacter::HandleLook(const  FInputActionValue& Value)
{

    const FVector2D Look = Value.Get<FVector2D>();
    // takes the input action that has now been renamed to "Value", retrieves the FVector2D that is stored inside that input action value.
   // then sets it equal to a constant FVector2D called Look. const means constant and promises not to change the value of the variable just read it 

    AddControllerYawInput(Look.X);
    // AddControllerYawInput: tells the Controller to rotate its ControlRotation around the Yaw axis (Z axis = left/right turning).
// This does not instantly rotate the actor itself. Instead, it accumulates yaw rotation input into the PlayerController.
// Since our SpringArm is set to "Use Pawn Control Rotation," the camera follows this updated rotation each frame.



    AddControllerPitchInput(Look.Y);
    // AddControllerPitchInput: tells the Controller to rotate its ControlRotation around the Pitch axis (Y axis = up/down looking).
   // Like yaw, this updates the Controller’s ControlRotation (not the pawn directly).
   // The SpringArm/Camera reads that rotation, which makes the camera tilt up and down.
}


void AAMazeCharacter::OnJumpPressed(const FInputActionValue& /*Value*/)
{
    Jump();            // tells CharacterMovement to perform a jump using JumpZVelocity
}

void AAMazeCharacter::OnJumpReleased(const FInputActionValue& /*Value*/)
{
    StopJumping();     // allows short-hopping when player releases early
}


void AAMazeCharacter::OnInteractPressed(const FInputActionValue& Value)
{

}


void AAMazeCharacter::OnUsePressed(const FInputActionValue& Value)
{

}
 



void AAMazeCharacter::OnItemSlotOnePressed(const FInputActionValue& Value)
{

}


void AAMazeCharacter::OnItemSlotTwoPressed(const FInputActionValue& Value)
{

}


void AAMazeCharacter::OnItemSlotThreePressed(const FInputActionValue& Value)
{

}

void AAMazeCharacter::SetupDefaultInputMapping()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsys->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }
    }
}


void AAMazeCharacter::DealDamage(float DamageAmount)
{
    if (currentHealth <= 0) // making sure we dont apply more damage if are players health is already zero so there dead 
    {
        return;
    }
    
    currentHealth -= DamageAmount; // subtracting the damage amount from health.

    UE_LOG(LogTemp, Warning, TEXT("Damage has been done"));
    if (currentHealth <= 0)
    {
        HandleDeath();
    }
}

void AAMazeCharacter::HandleDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("Death has been called"));
}