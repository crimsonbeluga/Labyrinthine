// InteractableDoor.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableDoor.generated.h"

class UStaticMeshComponent;
class UItemDef;
class UInventoryComponent;

UENUM(BlueprintType)
enum class EDoorAxis : uint8 { X, Y, Z };

UCLASS()
class LABYRINTHINE_API AInteractableDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableDoor();

	// IInteractable
	virtual FText GetPromptText_Implementation() const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	// API
	UFUNCTION(BlueprintCallable, Category = "Door") void OpenDoor();
	UFUNCTION(BlueprintCallable, Category = "Door") void CloseDoor();
	UFUNCTION(BlueprintCallable, Category = "Door") bool IsOpen() const { return bOpen; }

	// AActor
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	virtual void BeginPlay() override;

private:
	// Components (you place these in the BP; code never moves the frame)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorMesh;

	// Config
	UPROPERTY(EditAnywhere, Category = "Door|Motion", meta = (ClampMin = "0.01"))
	float OpenDurationSeconds = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Door|Motion")
	float OpenAngleDegrees = 90.f;

	// Hinge point relative to DoorMesh local origin (set this once per BP instance)
	UPROPERTY(EditAnywhere, Category = "Door|Motion")
	FVector HingeLocal = FVector(0, -50, 0); // e.g. left edge = negative Y if mesh pivot is center

	UPROPERTY(EditAnywhere, Category = "Door|Motion")
	EDoorAxis HingeAxis = EDoorAxis::Z;

	// Key gating
	UPROPERTY(EditAnywhere, Category = "Door|Key")
	bool bRequireKey = false;

	UPROPERTY(EditAnywhere, Category = "Door|Key", meta = (EditCondition = "bRequireKey"))
	UItemDef* RequiredKey = nullptr;

	UPROPERTY(EditAnywhere, Category = "Door|Key", meta = (EditCondition = "bRequireKey", ClampMin = "1"))
	int32 RequiredKeyCount = 1;

	UPROPERTY(EditAnywhere, Category = "Door|Key", meta = (EditCondition = "bRequireKey"))
	bool bConsumeKeyOnUnlock = false;

	UPROPERTY(EditAnywhere, Category = "Door|Key")
	bool bStartUnlocked = false;

	// Runtime
	bool  bUnlocked = false;
	bool  bOpen = false;
	bool  bAnimating = false;
	float Alpha = 0.f; // 0?closed, 1?open (monotonic per animation)

	FTransform ClosedWT; // cached at BeginPlay (or first construct)
	// Helpers
	FTransform MakeDoorWorldAtAlpha(float A) const;
	static FVector AxisWorld(const FTransform& Ref, EDoorAxis Axis);
};
