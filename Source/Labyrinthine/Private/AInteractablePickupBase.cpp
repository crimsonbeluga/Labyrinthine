#include "AInteractablePickupBase.h"           // matches file name exactly
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "InventoryComponent.h"
#include "ItemDef.h"

AAInteractablePickupBase::AAInteractablePickupBase()
{
	PrimaryActorTick.bCanEverTick = false; // pick ups dont need per frame updates so we disable this

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Optional floating prompt widget
	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Prompt"));
	PromptWidget->SetupAttachment(RootComponent);
	PromptWidget->SetVisibility(false); // hidden until focused

	Mesh->SetCollisionObjectType(ECC_WorldDynamic); //“This object is a movable, interactive world object (not part of the static world geometry).”
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Enables both queries(line traces, sweeps, overlaps) and physics collision You want traces to “see” the pickup(for focus / LOS) and you may want the mesh to physically

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	//What: Start from a “deny-all” baseline—ignore everything by default.
// It prevents accidental interactions with channels you don’t care about.
	//From this clean slate you whitelist only the channels that matter, which avoids weird side effects later.

	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	//What: Explicitly blocks your custom interaction trace channel.
	// Why: Your focus/interaction line trace uses this channel; blocking it ensures the trace hits the pickup 
	// (and that walls can occlude if they also block this channel). This is the backbone of “look-at to focus.”

	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // if you ever use Visibility traces

	//What: Also blocks the built-in Visibility channel.
//Why: If some systems(debug tools, UI line - of - sight checks, generic trace helpers) use Visibility instead of your custom channel, 
// the pickup will still be hittable / occluded properly. It’s optional—useful if any of your traces are on Visibility now or might be later.
}

/// Return the localized text prompt shown when the player looks at the item
FText AAInteractablePickupBase::GetPromptText_Implementation() const
{
	if (ItemData) // checking if item data exists
	{
		return FText::Format( // Formats a localized text string
			NSLOCTEXT("Interact", "PickUpFmt", "Pick up {0}"),
			ItemData->DisplayName
		);
	}

	// Fallback return (required so all code paths return a value)
	return NSLOCTEXT("Interact", "PickUp", "Pick up");
}

void AAInteractablePickupBase::Interact_Implementation(AActor* Interactor)
{

	if (!ItemData || !Interactor) // if we cant find the data on the item cause it hasentbeen assigned or we cantt find what interacted with us exit out of this early
	{
		return;
	}

	if (UInventoryComponent* Inv = Interactor->FindComponentByClass<UInventoryComponent>())
		// if we can find an inventory component onthe object that interaqctoed with us then assigne it a pointer call INV
	{
		if (Inv->AddItem(ItemData, Quantity)) // if we can find an inventory component then add the item by passing in what type of object and how mmuch of it
		{
			SetActorEnableCollision(false);
			SetActorHiddenInGame(true);
			Destroy();
		}
	}

}