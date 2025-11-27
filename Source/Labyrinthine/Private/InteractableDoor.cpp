// InteractableDoor.cpp
#include "InteractableDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "ItemDef.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AInteractableDoor::AInteractableDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(Root);
	DoorFrame->SetCollisionProfileName(TEXT("BlockAll"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Root);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
	DoorMesh->SetMobility(EComponentMobility::Movable);
}

void AInteractableDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// No transform edits here. Designers place DoorFrame and DoorMesh freely.
}

void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();
	bUnlocked = bStartUnlocked;
	ClosedWT = DoorMesh->GetComponentTransform(); // baseline “closed” pose
}

FText AInteractableDoor::GetPromptText_Implementation() const
{
	if (!bUnlocked && bRequireKey)
	{
		return RequiredKey
			? FText::Format(NSLOCTEXT("Interact", "LockedFmt", "Locked: {0}"), RequiredKey->DisplayName)
			: NSLOCTEXT("Interact", "Locked", "Locked");
	}
	return bOpen ? NSLOCTEXT("Interact", "Close", "Close door")
		: NSLOCTEXT("Interact", "Open", "Open door");
}

void AInteractableDoor::Interact_Implementation(AActor* Interactor)
{
	if (bAnimating) return;

	if (!bUnlocked && bRequireKey)
	{
		bool bHasKey = false;
		if (Interactor)
		{
			if (UInventoryComponent* Inv = Interactor->FindComponentByClass<UInventoryComponent>())
			{
				if (RequiredKey && RequiredKeyCount > 0)
				{
					bHasKey = Inv->HasItem(RequiredKey, RequiredKeyCount);
					if (bHasKey && bConsumeKeyOnUnlock) Inv->ConsumeItem(RequiredKey, RequiredKeyCount);
				}
			}
		}
		if (!bHasKey)
		{
			if (GEngine)
			{
				const FText Msg = RequiredKey
					? FText::Format(NSLOCTEXT("Interact", "NeedKeyFmt", "Requires {0}"), RequiredKey->DisplayName)
					: NSLOCTEXT("Interact", "NeedKey", "Door is locked");
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, Msg.ToString());
			}
			return;
		}
		bUnlocked = true;
	}

	if (bOpen) CloseDoor(); else OpenDoor();
}

void AInteractableDoor::OpenDoor()
{
	if (bAnimating) return;
	// Re-baseline in case designer moved it in editor while PIE
	ClosedWT = DoorMesh->GetComponentTransform();
	bAnimating = true;
	bOpen = true;
	Alpha = 0.f;
	SetActorTickEnabled(true);
	UGameplayStatics::PlaySoundAtLocation(this, OpenDoorSound, GetActorLocation());

}

void AInteractableDoor::CloseDoor()
{
	if (bAnimating) return;
	bAnimating = true;
	bOpen = false;
	Alpha = 1.f;
	SetActorTickEnabled(true);
	UGameplayStatics::PlaySoundAtLocation(this, CloseDoorSound, GetActorLocation());
}

static FQuat MakeDeltaQuat(const FVector& AxisW, float AngleDeg)
{
	return FQuat(AxisW.GetSafeNormal(), FMath::DegreesToRadians(AngleDeg));
}

FVector AInteractableDoor::AxisWorld(const FTransform& Ref, EDoorAxis Axis)
{
	switch (Axis)
	{
	case EDoorAxis::X: return Ref.GetUnitAxis(EAxis::X);
	case EDoorAxis::Y: return Ref.GetUnitAxis(EAxis::Y);
	default:           return Ref.GetUnitAxis(EAxis::Z);
	}
}

// A=0 => closed; A=1 => fully open
FTransform AInteractableDoor::MakeDoorWorldAtAlpha(float A) const
{
	const FTransform& Base = ClosedWT;
	const FVector PivotW = Base.TransformPosition(HingeLocal);
	const FVector AxisW = AxisWorld(Base, HingeAxis);
	const FQuat   DQ = MakeDeltaQuat(AxisW, OpenAngleDegrees * A);

	const FVector P0 = Base.GetLocation();
	const FVector Pn = PivotW + DQ.RotateVector(P0 - PivotW);
	const FQuat   Rn = DQ * Base.GetRotation();

	FTransform Out;
	Out.SetLocation(Pn);
	Out.SetRotation(Rn);
	Out.SetScale3D(Base.GetScale3D());
	return Out;
}

void AInteractableDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bAnimating) return;

	const float Speed = 1.f / FMath::Max(0.0001f, OpenDurationSeconds);
	Alpha += (bOpen ? +1.f : -1.f) * Speed * DeltaSeconds;
	const float Clamped = FMath::Clamp(Alpha, 0.f, 1.f);

	const FTransform NewWT = MakeDoorWorldAtAlpha(Clamped);
	DoorMesh->SetWorldTransform(NewWT, false, nullptr, ETeleportType::None);

	if ((bOpen && Clamped >= 1.f) || (!bOpen && Clamped <= 0.f))
	{
		bAnimating = false;
		SetActorTickEnabled(false);
	}
}
