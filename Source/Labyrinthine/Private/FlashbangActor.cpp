// FlashbangActor.cpp
#include "FlashbangActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AFlashbangActor::AFlashbangActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Physics setup
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	// Prevent the player character from pushing it around
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AFlashbangActor::BeginPlay()
{
	Super::BeginPlay();

	// Ensure owner cannot collide with us to avoid immediate impulse
	if (AActor* Owning = GetOwner())
	{
		Mesh->IgnoreActorWhenMoving(Owning, true);
	}
}

void AFlashbangActor::Arm(float FuseSeconds, AActor* NewInstigator)
{
	SetInstigator(Cast<APawn>(NewInstigator));

	const float Fuse = FMath::Max(0.05f, FuseSeconds);
	GetWorldTimerManager().SetTimer(FuseHandle, this, &AFlashbangActor::Detonate, Fuse, false);
}

void AFlashbangActor::Detonate()
{
	// FX + SFX
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorTransform(), true);
	}
	if (ExplosionSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, GetActorLocation());
	}

	// One-frame hitbox
	DoFlashHitbox();

	Destroy();
}

void AFlashbangActor::DoFlashHitbox()
{
	const FVector Center = GetActorLocation();
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes = {
		UEngineTypes::ConvertToObjectType(ECC_Pawn)  // check pawns only for now
	};
	TArray<AActor*> Ignore;
	if (AActor* Owning = GetOwner()) { Ignore.Add(Owning); }
	if (APawn* Inst = GetInstigator()) { Ignore.Add(Inst); }

	TArray<AActor*> Hits;
	const bool bAny = UKismetSystemLibrary::SphereOverlapActors(
		this,
		Center,
		FlashRadius,
		ObjTypes,
		AActor::StaticClass(),
		Ignore,
		Hits
	);

#if WITH_EDITOR
	if (DebugHitboxDrawSeconds > 0.f)
	{
		DrawDebugSphere(GetWorld(), Center, FlashRadius, 24, FColor::White, false, DebugHitboxDrawSeconds, 0, 1.5f);
	}
#endif

	if (!bAny) return;

	// For now we only broadcast presence. Hook your stun logic elsewhere.
	for (AActor* A : Hits)
	{
		// Placeholders:
		// - Add gameplay tag, call an interface, or set a blackboard value in your AI.
		// - Example: IFlashAffectable::Execute_OnFlashed(A, Duration);
		UE_LOG(LogTemp, Log, TEXT("[Flashbang] Overlap: %s"), *A->GetName());
	}
}
