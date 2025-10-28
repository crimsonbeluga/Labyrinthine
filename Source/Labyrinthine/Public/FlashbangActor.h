#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashbangActor.generated.h"

class UStaticMeshComponent;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class LABYRINTHINE_API AFlashbangActor : public AActor
{
	GENERATED_BODY()

public:
	AFlashbangActor();

	// Mesh with physics enabled
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	// Niagara VFX on explosion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* ExplosionFX = nullptr;

	// Audio on explosion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	USoundBase* ExplosionSFX = nullptr;

	// Radius of the flash hitbox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash", meta = (ClampMin = "0.0"))
	float FlashRadius = 900.f;

	// How long AI stays stunned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash", meta = (ClampMin = "0.0"))
	float StunDuration = 2.0f;

	// Optional debug draw time for the hitbox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	float DebugHitboxDrawSeconds = 0.15f;

	// Arm the grenade. Starts fuse timer.
	void Arm(float FuseSeconds, AActor* NewInstigator);

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle FuseHandle;

	void Detonate();
	void DoFlashHitbox();
	void ApplyStun(AActor* A) const;
};
