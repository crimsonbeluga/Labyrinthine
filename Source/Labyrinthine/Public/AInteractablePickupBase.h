#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "AInteractablePickupBase.generated.h" // ✅ must be the LAST include line

// Forward declarations
class UStaticMeshComponent;
class UWidgetComponent;
class UItemDef;

// This actor represents a physical item pickup in the world that can be interacted with by the player.
// It implements the IInteractable interface so that the player can look at it, see a prompt, and pick it up.
UCLASS()
class LABYRINTHINE_API AAInteractablePickupBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Constructor — sets up mesh, collision, and optional prompt widget.
	AAInteractablePickupBase();

protected:
	// Visible mesh representing the pickup object in the world (e.g., grenade, potion, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	// Optional widget that can display a "Press E to Pick Up" prompt.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* PromptWidget;

	// Reference to the item definition (UItemDef) that this pickup represents.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UItemDef* ItemData = nullptr;

	// Number of items this pickup gives when collected.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 Quantity = 1;

public:
	// =============================
	// IInteractable Interface Methods
	// =============================

	// Called when the player looks at this item — returns the text to display on-screen.
	virtual FText GetPromptText_Implementation() const override;

	// Called when the player interacts (presses Interact key).
	virtual void Interact_Implementation(AActor* Interactor) override;
};


