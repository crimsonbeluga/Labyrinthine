#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{
    GENERATED_BODY()

public:
   UFUNCTION(BlueprintNativeEvent, BlueprintCallable, category = "Interactable")
   // "UFUNCTION" is a macro that tells unreals reflectiin system that it is known and exists which lets lets Blueprints, replication, serialization, and the editor see it.
   // "UFUNCTION" is required so Unreal can wire up interface calls and Execute_ helpers.
// --------------------------------------------------------------------------------------
// BlueprintNativeEvent explanation:
//
// "BlueprintNativeEvent" tells Unreal that this function can be implemented in both C++ 
// and Blueprints. It creates two versions of the function internally:
//
//   1) Interact(AActor* Interactor);                 // The main callable version
//   2) Interact_Implementation(AActor* Interactor);  // The actual body you define in C++
//
// When UnrealHeaderTool processes this function, it automatically generates the 
// "_Implementation" version so you can write your logic in C++ while still allowing
// Blueprint subclasses to override it if desired.
//
// When you call the function through the Unreal reflection system (e.g., using 
// IInteractable::Execute_Interact(Target, this)), Unreal checks:
//
//   - If the object has a Blueprint override for "Interact", it runs that version.
//   - If not, it falls back to the C++ "_Implementation" version.
//
// In short, BlueprintNativeEvent provides flexible polymorphism between C++ and 
// Blueprint systems.
//
// Why we use it here:
//   • Unreal interfaces require UFUNCTION() for reflection and dynamic dispatch.
//   • BlueprintNativeEvent automatically generates the "Execute_" helper functions 
//     (like Execute_Interact) used in interface calls.
//   • Even if this project is 100% C++, using BlueprintNativeEvent allows future 
//     Blueprint subclasses (like a door or pickup actor) to override the interaction 
//     without editing code.
//   • This specifier is NOT for replication. RPCs (Server/Client/Multicast) belong 
//     on UActorComponent or AActor functions, not interfaces.


   FText GetPromptText() const;
   // returns the text prompt 

   UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
   void Interact(AActor* Interactor);

};
