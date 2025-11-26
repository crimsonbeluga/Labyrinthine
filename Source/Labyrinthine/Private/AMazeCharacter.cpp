// AMazeCharacter.cpp
#include "AMazeCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"


// Enhanced Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

// Tracing
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

// Interactions
#include "Interactable.h"

// Inventory + items
#include "InventoryComponent.h"
#include "ItemDef.h"
#include "ItemUseBehavior.h"

AAMazeCharacter::AAMazeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	// Movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	// Interaction
	InteractRange = CreateDefaultSubobject<USphereComponent>(TEXT("Interact Range"));
	InteractRange->InitSphereRadius(180.f);
	InteractRange->SetupAttachment(RootComponent);
	InteractRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractRange->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AAMazeCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupDefaultInputMapping();
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
		if (IA_Move)
		{
			EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAMazeCharacter::HandleMove);
			EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AAMazeCharacter::HandleMove);
		}
		if (IA_Look)
		{
			EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AAMazeCharacter::HandleLook);
		}
		if (IA_Jump)
		{
			EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AAMazeCharacter::OnJumpPressed);
			EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AAMazeCharacter::OnJumpReleased);
		}
		if (IA_Interact)
		{
			EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AAMazeCharacter::OnInteractPressed);
		}
		if (IA_Use)
		{
			EIC->BindAction(IA_Use, ETriggerEvent::Started, this, &AAMazeCharacter::OnUsePressed);
		}
		if (IA_ItemSlot_1)
		{
			EIC->BindAction(IA_ItemSlot_1, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotOnePressed);
		}
		if (IA_ItemSlot_2)
		{
			EIC->BindAction(IA_ItemSlot_2, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotTwoPressed);
		}
		if (IA_ItemSlot_3)
		{
			EIC->BindAction(IA_ItemSlot_3, ETriggerEvent::Started, this, &AAMazeCharacter::OnItemSlotThreePressed);
		}
		if (IA_Pause)
		{
			EIC->BindAction(IA_Pause, ETriggerEvent::Started, this, &AAMazeCharacter::OnPausePressed);
		}
		if (IA_Sprint)
		{
			EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AAMazeCharacter::SetSprintSpeed);
			EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AAMazeCharacter::SetWalkSpeed);
		}



	}
}

void AAMazeCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (!Controller || Input.IsNearlyZero()) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Input.X);
	AddMovementInput(Right, Input.Y);
}

void AAMazeCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D Look = Value.Get<FVector2D>();
	AddControllerYawInput(Look.X);
	AddControllerPitchInput(Look.Y);
}

void AAMazeCharacter::OnJumpPressed(const FInputActionValue& /*Value*/)
{
	Jump();
}

void AAMazeCharacter::OnJumpReleased(const FInputActionValue& /*Value*/)
{
	StopJumping();
}

void AAMazeCharacter::OnInteractPressed(const FInputActionValue& /*Value*/)
{
	Inventory->OnInventoryChanged.Broadcast();

	AActor* Target = LastInteractHit.GetActor();

	if (!Target)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("No valid interact hit"));
		return;
	}

	if (!Target->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Hit non-interactable object"));
		return;
	}

	IInteractable::Execute_Interact(Target, this);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
			FString::Printf(TEXT("Interacted with: %s"), *Target->GetName()));
	}
}

void AAMazeCharacter::OnUsePressed(const FInputActionValue& /*Value*/)
{
	Inventory->OnInventoryChanged.Broadcast();

	if (Inventory) { Inventory->UseActive(this); }
}

void AAMazeCharacter::OnItemSlotOnePressed(const FInputActionValue& /*Value*/)
{

	 if(Inventory)
	 {
		 Inventory->SelectSlot(0);

	 }
}

void AAMazeCharacter::OnItemSlotTwoPressed(const FInputActionValue& /*Value*/)
{

	if (Inventory)
	{
		Inventory->SelectSlot(1);
	}
}

void AAMazeCharacter::OnItemSlotThreePressed(const FInputActionValue& /*Value*/)
{

	if (Inventory)
	{
		Inventory->SelectSlot(2);
	}
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

	const float StartOffset = 0.f;
	const float Distance = InteractTraceDistance;
	const float Radius = InteractTraceRadius;

	const FVector CamPos = Camera->GetComponentLocation();
	const FVector CamForward = Camera->GetComponentRotation().Vector();
	const FVector Start = CamPos + CamForward * StartOffset;
	const FVector End = Start + CamForward * Distance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractSweep), true, this);
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	LastInteractHit = FHitResult();

	bool bFoundInteractable = false; // NEW

	const bool bAny = GetWorld()->SweepMultiByChannel(
		Hits, Start, End, FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (bAny) // if the trace hit anything
	{
		for (const FHitResult& H : Hits) // for every hit object 
		{
			if (!(H.bBlockingHit || H.bStartPenetrating))
				continue;

			AActor* A = H.GetActor(); // store the hit object in an actor pointer called A
			if (!A) continue;

			if (A->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				// creating a prompt of class FText thats gonna store what it says by calling 
				// the get prompt text function from the specific actor we have hit that implements it
				const FText Prompt = IInteractable::Execute_GetPromptText(A);
				// The A in the parenthesis is specifying which object that implements
				// interactable we are calling this function on

				// NEW: tell Blueprint to show/update the UI prompt
				ShowInteractionPrompt(Prompt);

				// widget activation function call would go here
				LastInteractHit = H;
				bFoundInteractable = true;    // NEW

				break;
			}
		}
	}

	// NEW: if we didn't find any interactable this frame, hide the prompt
	if (!bFoundInteractable)
	{
		HideInteractionPrompt();
	}

#if WITH_EDITOR
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
#endif
}


bool AAMazeCharacter::IsWithinInteractRange(const AActor* Target) const
{
	if (!Target) return false;
	const float Range = InteractRange ? InteractRange->GetScaledSphereRadius() : 100.f;
	const float DistSq = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
	return DistSq <= FMath::Square(Range);
}

void AAMazeCharacter::DealDamage(float DamageAmount)
{
	if (currentHealth <= 0) return;

	currentHealth -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("Damage has been done"));
	if (currentHealth <= 0)
	{
		HandleDeath();
	}
}

void AAMazeCharacter::AddHealth(float AddedHealthAmount)
{
	if (currentHealth <= 0) return;

	

	currentHealth += AddedHealthAmount;

	currentHealth = FMath::Clamp(currentHealth, 0.0, 100);

	UE_LOG(LogTemp, Warning, TEXT("Health increased by %f"), AddedHealthAmount);

}

void AAMazeCharacter::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Death has been called"));
}

void AAMazeCharacter::TryUseSelectedItem()
{
	if (Inventory) { Inventory->UseActive(this); }
}


void AAMazeCharacter::SetSprintSpeed()
{
	if (!bCanSprint) return;
	bIsWalking = false;

	GetWorld()->GetTimerManager().SetTimer(
		sprintTimerHandle,
		this,
		&AAMazeCharacter::SetWalkSpeed, // <- ampersand is required
		sprintTime,
		false
	);

	GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;

	
}


void AAMazeCharacter::SetWalkSpeed()
{
	if (bIsWalking)
	{
		return;
	}

	DisableSprintiing();
	SprintRecoveryTimeMath();
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	bIsWalking = true;

}

void AAMazeCharacter::DisableSprintiing()
{
	bCanSprint = false;
	
}

void AAMazeCharacter::EnableSprint()
{
	bCanSprint = true;
	

}

void AAMazeCharacter::SprintRecoveryTimeMath()
{
	timeSprinted = GetWorld()->GetTimerManager().GetTimerElapsed(sprintTimerHandle);



	GetWorld()->GetTimerManager().SetTimer(
		sprintRecoveryTimerHandle,
		this,
		&AAMazeCharacter::EnableSprint, // <- ampersand is required
		timeSprinted,
		false
	);

}