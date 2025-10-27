// InteractableDoor.h
// Door actor with static frame + hinged door leaf.
// Implements IInteractable and supports optional key requirement via UItemDef (e.g., DA_Key_Silver).
// Non-intrusive construction: we DO NOT move your DoorFrame/HingePivot unless you opt in.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UItemDef;

UENUM(BlueprintType)
enum class EDoorHingeSide : uint8
{
	Left  UMETA(DisplayName = "Left (hinge on +Y)"),
	Right UMETA(DisplayName = "Right (hinge on -Y)")
};

UCLASS()
class LABYRINTHINE_API AInteractableDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableDoor();

	// IInteractable
	virtual FText GetPromptText_Implementation() const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	// Door control
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	// AActor
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	// Auto-offset the leaf so its hinge edge sits on the pivot (safe + minimal)
	void ApplyHingeOffset();

protected:
	// ----- Components -----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorFrame = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HingePivot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorMesh = nullptr;

	// ----- Hinge config -----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Hinge")
	bool bAutoSetupHinge = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Hinge")
	EDoorHingeSide HingeSide = EDoorHingeSide::Left;

	// ----- Open animation -----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Open")
	float OpenAngleDegrees = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Open", meta = (ClampMin = "0.01"))
	float OpenDurationSeconds = 1.0f;

	// Frame mobility (static recommended)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Frame")
	bool bFrameIsStatic = true;

	// ----- Optional frame recenter (OFF by default) -----
	// If true, we’ll recenter the frame by its mesh bounds (fix for bad mesh pivots).
	// DEFAULT false so we never override your manual placement.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Frame")
	bool bAutoRecenterFrame = false;

	// Optional nudge after recenter (only used if bAutoRecenterFrame = true)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Frame", meta = (EditCondition = "bAutoRecenterFrame", EditConditionHides))
	FVector FrameOffset = FVector::ZeroVector;

	// ----- Key requirement (optional) -----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key")
	bool bRequireKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key", meta = (EditCondition = "bRequireKey", EditConditionHides))
	UItemDef* RequiredKey = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key", meta = (ClampMin = "1", EditCondition = "bRequireKey", EditConditionHides))
	int32 RequiredKeyCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key", meta = (EditCondition = "bRequireKey", EditConditionHides))
	bool bConsumeKeyOnOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key")
	bool bStartUnlocked = false;

private:
	// ----- Runtime state -----
	FRotator PivotStartRot = FRotator::ZeroRotator;
	FRotator PivotTargetRot = FRotator::ZeroRotator;
	float    OpenAlpha = 0.f;
	bool     bIsOpening = false;

	bool bUnlocked = false;
	bool bHingeOffsetApplied = false;
};
