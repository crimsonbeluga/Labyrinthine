// FlashbangActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashbangActor.generated.h"

class UStaticMeshComponent;
class USoundBase;
class UParticleSystem;

UCLASS()
class LABYRINTHINE_API AFlashbangActor : public AActor
{
	GENERATED_BODY()

public:
	AFlashbangActor();

	// Mesh with physics enabled
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	// Visual and audio on explosion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* ExplosionFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	USoundBase* ExplosionSFX = nullptr;

	// Radius of the flash hitbox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash", meta = (ClampMin = "0.0"))
	float FlashRadius = 900.f;

	// Optional debug draw time for the hitbox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	float DebugHitboxDrawSeconds = 0.15f;

	// Arm the grenade. Starts fuse timer.
	void Arm(float FuseSeconds, AActor* NewInstigator);

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle FuseHandle;

	// Called by timer
	void Detonate();

	// Utility: overlaps in radius for a single frame and can be extended later
	void DoFlashHitbox();
};
