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

//ray usage
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

//interactions
#include "Interactable.h"

#include "InventoryComponent.h"




// Sets default values
AAMazeCharacter::AAMazeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Camera Category
    // -------------------------------------------------------------------------------
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent); // This is valid now because of BasePawn
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

    //Input Category
     // -------------------------------------------------------------------------------
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

    //Interaction Category
     // -------------------------------------------------------------------------------
    InteractRange = CreateDefaultSubobject<USphereComponent>(TEXT("Interact Range")); // creating the sphere component we will use to detect if we are within interaction range
    InteractRange->InitSphereRadius(180.f); // how large the initial size of the sphere will be // we can just change it in the blueprint later
    InteractRange->SetupAttachment(RootComponent); // making the sphere component a child of the root component
    InteractRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  
    // were Setting this sphere’s collision mode to QueryOnly, so it participates in raycasts and overlaps — but not physics blocking.”

    InteractRange->SetCollisionResponseToAllChannels(ECR_Ignore); 
    // Set the InteractRange sphere to ignore all collision channels by default.
// (We'll selectively enable overlaps for specific channels later, e.g., interactables or pawns.)

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));




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
    UpdateInteractionFocus();

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
            EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AAMazeCharacter::OnInteractPressed);
        }

        if (IA_Use)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_Use, ETriggerEvent::Started, this, &AAMazeCharacter::OnUsePressed);
        }

        if (IA_ItemSlot_1)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_1, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotOnePressed);
        }

        if (IA_ItemSlot_2)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_2, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotTwoPressed);
        }

        if (IA_ItemSlot_3)//if its assigned in the AMazeCharacter blue print
        {
            EIC->BindAction(IA_ItemSlot_3, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotThreePressed);
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
    AActor* Target = LastInteractHit.GetActor();

    if (!Target)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("No valid interact hit"));
        return;
    }

    if (!Target->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Hit non-interactable object"));
        return;
    }

    IInteractable::Execute_Interact(Target, this);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
            FString::Printf(TEXT("Interacted with: %s"), *Target->GetName()));
    }
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


void AAMazeCharacter::UpdateInteractionFocus()
{
    if (!Camera) return;

    const float StartOffset = 200.f;
    const float Distance = InteractTraceDistance;
    const float Radius = InteractTraceRadius;

    const FVector CamPos = Camera->GetComponentLocation();
    const FVector CamForward = Camera->GetComponentRotation().Vector();
    const FVector Start = CamPos + CamForward * StartOffset;
    const FVector End = Start + CamForward * Distance;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractSweep), true, this);
    Params.AddIgnoredActor(this);

    TArray<FHitResult> Hits;
    LastInteractHit = FHitResult(); // reset each frame

    const bool bAny = GetWorld()->SweepMultiByChannel(
        Hits, Start, End, FQuat::Identity,
        ECC_GameTraceChannel1,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    if (bAny)
    {
        // We'll take the first blocking hit that implements Interactable
        for (const FHitResult& H : Hits)
        {
            if (!(H.bBlockingHit || H.bStartPenetrating))
                continue;

            AActor* A = H.GetActor();
            if (!A) continue;

            if (A->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
            {
                LastInteractHit = H;
                break; // found a valid interactable hit
            }
        }
    }

#if WITH_EDITOR
    // ---------------- Debug viz ----------------
    {
        const FVector Path = End - Start;
        const float PathLen = Path.Size();
        const FVector Mid = Start + Path * 0.5f;
        const FQuat Rot = FRotationMatrix::MakeFromZ(Path.GetSafeNormal()).ToQuat();

        DrawDebugCapsule(GetWorld(), Mid, PathLen * 0.5f, Radius, Rot,
            FColor(80, 80, 80), false, 0.f, 0, 1.25f);

        for (const FHitResult& H : Hits)
        {
            if (H.bBlockingHit || H.bStartPenetrating)
            {
                DrawDebugSphere(GetWorld(), H.ImpactPoint, 6.f, 12,
                    (AActor*)H.GetActor() == (AActor*)LastInteractHit.GetActor() ?
                    FColor::Green : FColor::Yellow,
                    false, 0.f, 0, 1.25f);
            }
        }
    }
#endif
}






bool AAMazeCharacter::IsWithinInteractRange(const AActor* Target) const
// Function that checks whether the focused actor(are interactable object) is close enough to  be interacted with.
// Returns true if the actor is within the defined interaction range; false otherwise.
// -------------------------------------------------------------------------------
// (const AActor* Target)→ The function parameter list. It takes one argument:
// - the first  const          → The function promises not to modify the actor passed in.
//  the last const → Function-level const qualifier. This means the function itself does not modify any member variables of this class (it’s a read-only check).    
{
    if (!Target) return false; // if there is no interactable object just immediately exit out of the function
    const float Range = InteractRange ? InteractRange->GetScaledSphereRadius() : 100.f;
    // create a const float called range so it promises not to be changed once set
    // inside the range float were checking if the interact range ( are sphere collider) exists on the game object
    // if it does were gonna call "GetScaledSphereRadius" which is gonna give us the radius of the sphere with the scale of the component applied and pass it as the float range
    // if it doesnt were setting the range float to 100                     

    //SOMETHING to REMEMBER: Because a sphere (in 3D) or a circle (in 2D) has a uniform distance from its center to any point on its surface,
// that distance is the radius — one consistent value in every direction. So when Unreal gives you the sphere’s radius as a single float,
//that number already represents “how far outward from the center the influence extends.”   That’s why this works so well for interaction checks
// — no matter which direction the player or target is in, if it’s within that radius, it’s considered “inside range.”


    const float DistSq = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
    // Compute the **squared Euclidean distance** between two 3D points without taking a square root.
// Math basis (Pythagorean theorem in 3D):
//   If A = (Ax, Ay, Az) and B = (Bx, By, Bz), then the true distance |A - B| is:
//       sqrt( (Ax - Bx)^2 + (Ay - By)^2 + (Az - Bz)^2 )
//   The **squared distance** (what we compute here) omits the sqrt:
//       (Ax - Bx)^2 + (Ay - Bz)^2 + (Az - Bz)^2
// Why squared? Taking sqrt is relatively expensive; for **range checks** we only need
// a comparison and can use the monotonic property of sqrt: for non-negative x,y,
//   sqrt(x) <= R  ⇔  x <= R^2
// So we compare DistSq to R^2 instead of comparing sqrt(DistSq) to R.
// Concretely here:
//   - Target->GetActorLocation() returns Target position T = (Tx,Ty,Tz) in Unreal units (cm).
//   - GetActorLocation() returns our position P = (Px,Py,Pz).
//   - FVector::DistSquared(T, P) computes (Tx-Px)^2 + (Ty-Py)^2 + (Tz-Pz)^2.
// Use case:
//   if (DistSq <= FMath::Square(Range)) { /* within Range centimeters */ }
// Notes:
//   • Faster than FVector::Dist(...) because no sqrt.
//   • Appropriate whenever you only need to **compare** distances (proximity, nearest, sorting).
//   • If you need a human-readable distance (e.g., to display), then compute the sqrt via Dist(...).
   
    return DistSq <= FMath::Square(Range);
    // We're returning whether the target is within our interaction range.
// DistSq is already a *squared distance* between our actor and the target, measured in cm².
// Range, however, is our actual radius (in cm), not squared.
// To compare them fairly, both values must be in the same "unit space" (cm²).
// Instead of taking a square root of DistSq (which is slower), we square the Range instead.
// This avoids the costly sqrt() operation while giving the exact same true/false result.
// So this line efficiently checks: "Is the target within Range units of us?" without extra computation.
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