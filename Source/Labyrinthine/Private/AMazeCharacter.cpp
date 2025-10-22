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
    if (!FocusedActor.IsValid())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("No interactable in focus"));
        }
        return;
    }

    // (Optional) show what we’re interacting with
    if (GEngine)
    {
        const FString Name = FocusedActor->GetName();
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
            FString::Printf(TEXT("Interact with: %s"), *Name));
    }

    IInteractable::Execute_Interact(FocusedActor.Get(), /* Interactor: */ this); // call the interact funtion   on the specific ineractable object
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

    if (!Camera) // if there is no camera 
    {
        FocusedActor = nullptr; 
        // set the pointer that stiores the interactable object were looking at (focused actor) to null cause we cant use the camera as the point to start the  ray to see what the [layer is looking at


        return; // bail out of the function early cause of the reason mentioned right above this/
    }

    const FVector Start = Camera->GetComponentLocation(); // creating a vector to store the location of the camera every frame  cause this will be the origin point of are ray


    const FVector End = Start + (Camera->GetComponentRotation().Vector() * 350.f);
    // Determine where the line trace (raycast) should end in world space.
// 1️⃣ Start with the camera's current world position as the origin of the ray.
// 2️⃣ Get the camera's current world space rotation and convert it to a normalized forward direction vector in world space using .Vector().
// .Vector() simply takes that worldspace rotation of the camera here and turns it into a unit forward direction vector that points in that same world direction.
//     - This gives us the direction the player is looking, with a length of 1 unit.
// so if for example there looking right  the vector would be (0,1,0) 
//     // In Unreal's coordinate system, X = forward, Y = right, and Z = up.
// So a direction vector of (0, 1, 0) means no forward or upward movement,
// only movement 1 unit to the right along the Y axis.
// 3️⃣ Multiply that direction by 350.f to extend the vector 350 Unreal units (≈3.5 meters) forward.
//     - This defines how far ahead the player can interact.
// 4️⃣ Add that forward offset to the camera's starting position to get the final endpoint in world space.
//     - The result is the "End" point of the trace — 350 units directly in front of the player's view.

    FHitResult Hit;
    // Create an empty FHitResult struct named "Hit".
// This struct will be filled with detailed information if our line trace collides with something.
// It stores data such as the actor that was hit, the exact impact point, the surface normal,
// the hit distance, and whether the trace was blocked or just overlapped.
// FHitResult is a built-in Unreal struct that contains variables for all the data a trace can return,
// such as the hit location, impact point, surface normal, hit actor, component, distance, and more.
// By declaring our own instance here, we allocate space for those variables,
// allowing Unreal's line trace function to automatically fill this struct(class)  with detailed hit information.

    FCollisionQueryParams Params(TEXT("InteractTrace"), false, this);
        // Create a new FCollisionQueryParams instance named "Params" by calling its constructor.
 // In C++, anytime you place parentheses () immediately after a type name during variable creation,
 // you are calling that type’s constructor and passing the values inside the parentheses as arguments.
 //
 // FCollisionQueryParams is a struct that stores settings for how a collision or line trace should behave.
 // Its constructor takes three parameters:
 //   1. TEXT("InteractTrace") → a name tag for identifying this trace in debugging or logs.
 //   2. false → disables complex (per-triangle) tracing and instead uses simple collision for performance.
 //   3. this → tells Unreal to ignore the actor running the trace, preventing it from hitting itself.
 //
 // Behind the scenes, this line calls FCollisionQueryParams::FCollisionQueryParams(...)
 // and initializes all of the struct’s internal variables accordingly.
    Params.AddIgnoredActor(this); // this does nothing new its just reduntant to exactly what we did right above. this will likely be removed

    const ECollisionChannel Channel = ECC_GameTraceChannel1;
    // Use our custom InteractTrace channel (ECC_GameTraceChannel1) instead of ECC_Visibility.
// Rationale:
// - Isolation: keeps interaction raycasts separate from other systems that also use Visibility
//   (AI sight, camera checks, UI traces), avoiding accidental behavior changes.
// - Control: we can fine-tune responses per asset—pickups/doors/walls “Block” InteractTrace,
//   the player and irrelevant actors “Ignore” it—without touching their Visibility settings.
// - LOS enforcement: because world geometry Blocks InteractTrace, the ray can’t pass through walls,
//   preventing “pickup through wall” bugs.
// - Scalability & clarity: future debugging and content setup are simpler (“Does it block InteractTrace?”)
//   and designers can tweak this channel without side effects elsewhere.

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, Params);  //THIS IS THE ACTUAL LINE THAT DOES THE RAY CAST CREATING THE LINE
// So this creates a boolean variable that will store whether the trace hit something or not.


// GetWorld()
// 'GetWorld()' is a built-in Unreal function that returns a pointer to the current UWorld object.
// The UWorld represents the entire game world (level, actors, physics, etc.).
// From the world object, we can call functions that perform world-level operations like traces and spawning actors.
// LineTraceSingleByChannel(...)
// This is the actual function being called on the UWorld object.
// It performs a *raycast* (a line trace) through the world along a line between two points (Start → End).
// It checks what objects (if any) the line collides with, using the specified collision channel.

// (Hit, Start, End, Channel, Params)
// These are the function’s parameters, passed in order:
// 1️⃣ 'Hit' — an output variable (FHitResult) that stores detailed info about what the trace hit
//             (like the actor, impact point, normal, distance, etc.).
// 2️⃣ 'Start' — the starting location of the line trace in world space (usually the camera or character position).
// 3️⃣ 'End' — the ending location of the trace (typically Start + ForwardVector * TraceDistance).
// 4️⃣ 'Channel' — specifies which collision channel to use (e.g., ECC_Visibility, ECC_InteractTrace).
//                 This tells the engine which objects to consider as "traceable".
// 5️⃣ 'Params' — a struct (FCollisionQueryParams) that defines extra options for the trace,
//                such as which actors to ignore, whether to use complex tracing, etc.
// Altogether, this line:
// Sends out a single invisible ray (line trace) from 'Start' to 'End' in the world,
// checking for the first object that blocks the specified collision 'Channel'.
//     "LineTraceSingleByChannel" Returns a bool  its one of the parameters of the function  so EX: bool UWorld::LineTraceSingleByChannel()
// If something is hit, 'Hit' is filled with data about it, and 'bHit' is set to true; otherwise, it's false.


#if WITH_EDITOR
    // #if WITH_EDITOR is a preprocessor directive used by Unreal Engine.
// It checks whether the code is being compiled in the Unreal Editor build configuration.
// If WITH_EDITOR is true (inside the editor), the enclosed code is included and compiled.
// If it's false (in a packaged/shipping build), the compiler completely skips this block.
// This is typically used to include editor-only or debug-only code,
// such as DrawDebugLine or DrawDebugPoint, so those visualizations
// appear in the editor but are excluded from the final game for performance and cleanliness.

    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0,1.5f);
    // were gonna create a debug line
    // calling get world tells the debug function what it needs to know (what world were in so it knows where its going)
    // we then pass in start as the beginning position of the line
    // we then pass in end and the end postion of the line,
    // we then do an iff check that says if we did hit something set the color to green if we didnt set it to red. 
    // we then pass in false for persisitent lines. 
    // we then pass in a float for its life time how long we want it to last in this case zero cause it is constanly just gonna be updated with tick  we dont want lingering lines
    // we then pass in its depth priotoity as zero i assume this means firs priority
    // and lastly we pass in its thickness to determine how large we want the line to be


    if (bHit) // if our line ray cast hit something
    {
        DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Yellow, false, 0.f, 0);
        // we're going to create a debug point
   // calling GetWorld() again tells the debug function which world we're drawing in
   // we then pass in Hit.ImpactPoint as the location where the line trace actually hit something in the world
   // we then pass in 8.f to define the size of the point (how large the visualized dot should appear)
   // we then pass in FColor::Yellow to color the point so it stands out from the line
   // we then pass in false for persistent lines, meaning we don't want it to stay in the world after this frame
   // we then pass in 0.f for lifetime, because we only want it visible for one tick before being redrawn again
   // lastly, we pass in 0 for depth priority, which defines its render order compared to other debug elements
    }




#endif //#endif simply marks the end of a conditional preprocessor block that started with #if, #ifdef, or #ifndef.


    AActor* NewCandidate = bHit ? Hit.GetActor() : nullptr;
    // creates a pointer to AActor called bHit
    // if bHit is true (our ray cast hit an object with in are defined channel so presumably it hit an interactable object)  
    // then we will use the hit result struct named Hit  that stores info aboutthe hit object to call the get actr function that will return the memory addres of the hit actor
    // then store it in the NewCandidate pointer
    // if bHit was not true (our ray cast did not hit an object with in are defined channel so presumably it did not hit an interactable object)
    // then the pointer will be set to null to avoid any errors


    if (NewCandidate && !IsWithinInteractRange(NewCandidate))
    {
        NewCandidate = nullptr;
    }

    FocusedActor = NewCandidate;

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