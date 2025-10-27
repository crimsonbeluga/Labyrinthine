// InteractableDoor.cpp
// Non-intrusive construction (we don't stomp your transforms), optional frame recenter, hinge auto-offset, key gating, slerp anim.

#include "InteractableDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ItemDef.h"
#include "InventoryComponent.h"

AInteractableDoor::AInteractableDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(Root);
	DoorFrame->SetCollisionProfileName(TEXT("BlockAll"));

	HingePivot = CreateDefaultSubobject<USceneComponent>(TEXT("HingePivot"));
	HingePivot->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(HingePivot);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

	DoorFrame->SetMobility(EComponentMobility::Static);
	DoorMesh->SetMobility(EComponentMobility::Movable);

	bUnlocked = bStartUnlocked;
}

FText AInteractableDoor::GetPromptText_Implementation() const
{
	if (bRequireKey && !bUnlocked)
	{
		if (RequiredKey)
		{
			return FText::Format(
				NSLOCTEXT("Interact", "DoorLockedFmt", "Locked: {0}"),
				RequiredKey->DisplayName
			);
		}
		return NSLOCTEXT("Interact", "DoorLocked", "Locked");
	}

	return NSLOCTEXT("Interact", "DoorOpen", "Open door");
}

void AInteractableDoor::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpening)
		return;

	if (bRequireKey && !bUnlocked)
	{
		bool bHasKey = false;

		if (Interactor)
		{
			if (UInventoryComponent* Inv = Interactor->FindComponentByClass<UInventoryComponent>())
			{
				if (RequiredKey && RequiredKeyCount > 0)
				{
					bHasKey = Inv->HasItem(RequiredKey, RequiredKeyCount);
					if (bHasKey && bConsumeKeyOnOpen)
					{
						Inv->ConsumeItem(RequiredKey, RequiredKeyCount);
					}
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

	OpenDoor();
}

void AInteractableDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// If any critical component is missing on a placed instance, bail safely.
	if (!Root || !DoorFrame || !HingePivot || !DoorMesh)
	{
		return;
	}

#if WITH_EDITOR
	DoorFrame->SetMobility(bFrameIsStatic ? EComponentMobility::Static : EComponentMobility::Movable);
	DoorMesh->SetMobility(EComponentMobility::Movable);
#endif

	// Keep lock state synced when editing
	bUnlocked = bStartUnlocked;

	// IMPORTANT: DO NOT stomp your manual placement.
	// We won't reset relative transforms anymore.
	// We only do optional frame recenter if you explicitly enable it:
	if (bAutoRecenterFrame)
	{
		if (UStaticMesh* FrameSM = DoorFrame->GetStaticMesh())
		{
			const FBoxSphereBounds FB = FrameSM->GetBounds();   // local-space bounds
			const FVector Center = FB.Origin;
			// Recenter around mesh bounds origin (safe; still keeps your existing relative transform)
			// We do this by setting a new relative location = (current - Center) + FrameOffset.
			const FVector CurrentRel = DoorFrame->GetRelativeLocation();
			DoorFrame->SetRelativeLocation(CurrentRel - Center + FrameOffset, false, nullptr, ETeleportType::ResetPhysics);
		}
	}

	// Auto hinge offset (leaf only; safe to leave on)
	bHingeOffsetApplied = false;
	if (bAutoSetupHinge)
	{
		if (UStaticMesh* LeafSM = DoorMesh->GetStaticMesh())
		{
			const FBoxSphereBounds LB = LeafSM->GetBounds();
			const float HalfWidthY = LB.BoxExtent.Y;
			if (HalfWidthY > KINDA_SMALL_NUMBER)
			{
				const float DesiredY = (HingeSide == EDoorHingeSide::Left) ? +HalfWidthY : -HalfWidthY;
				FVector Rel = DoorMesh->GetRelativeLocation();
				if (!FMath::IsNearlyEqual(Rel.Y, DesiredY, 0.1f))
				{
					Rel.Y = DesiredY;
					DoorMesh->SetRelativeLocation(Rel, false, nullptr, ETeleportType::ResetPhysics);
				}
				bHingeOffsetApplied = true;
			}
		}
	}
}

void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();
	bUnlocked = bStartUnlocked;

	// Runtime: hinge offset only (non-intrusive)
	ApplyHingeOffset();
}

void AInteractableDoor::ApplyHingeOffset()
{
	if (!bAutoSetupHinge || !DoorMesh)
		return;

	if (UStaticMesh* SM = DoorMesh->GetStaticMesh())
	{
		const FBoxSphereBounds LocalBounds = SM->GetBounds();
		const float HalfWidthY = LocalBounds.BoxExtent.Y;
		if (HalfWidthY <= KINDA_SMALL_NUMBER)
			return;

		const float OffsetY = (HingeSide == EDoorHingeSide::Left) ? +HalfWidthY : -HalfWidthY;
		FVector RelLoc = DoorMesh->GetRelativeLocation();
		RelLoc.Y = OffsetY;
		DoorMesh->SetRelativeLocation(RelLoc, false, nullptr, ETeleportType::ResetPhysics);
	}
}

void AInteractableDoor::OpenDoor()
{
	if (!HingePivot)
		return;

	if (bIsOpening || OpenDurationSeconds <= 0.f)
		return;

	bIsOpening = true;
	OpenAlpha = 0.f;
	PivotStartRot = HingePivot->GetRelativeRotation();
	PivotTargetRot = PivotStartRot + FRotator(0.f, OpenAngleDegrees, 0.f);
	SetActorTickEnabled(true);
}

void AInteractableDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsOpening || !HingePivot)
		return;

	OpenAlpha += (DeltaSeconds / OpenDurationSeconds);
	const float T = FMath::Clamp(OpenAlpha, 0.f, 1.f);

	const FQuat StartQ = PivotStartRot.Quaternion();
	const FQuat TargetQ = PivotTargetRot.Quaternion();
	const FQuat NewQ = FQuat::Slerp(StartQ, TargetQ, T);

	HingePivot->SetRelativeRotation(NewQ);

	if (T >= 1.f)
	{
		bIsOpening = false;
		SetActorTickEnabled(false);
	}
}
