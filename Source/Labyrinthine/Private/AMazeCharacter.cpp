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
	LastInteractHit = FHitResult();

	const bool bAny = GetWorld()->SweepMultiByChannel(
		Hits, Start, End, FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (bAny)
	{
		for (const FHitResult& H : Hits)
		{
			if (!(H.bBlockingHit || H.bStartPenetrating))
				continue;

			AActor* A = H.GetActor();
			if (!A) continue;

			if (A->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				LastInteractHit = H;
				break;
			}
		}
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
