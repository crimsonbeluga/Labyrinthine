// FlashbangActor.cpp
#include "FlashbangActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Niagara
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// AI/Blackboard
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionFX,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.f),
			true,   // auto destroy
			true,   // auto activate
			ENCPoolMethod::AutoRelease
		);
	}

	if (ExplosionSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, GetActorLocation());
	}

	// One-frame hitbox
	DoFlashHitbox();

	Destroy();
}

void AFlashbangActor::Throw(const FVector& ThrowDirection)
{
	if (!Mesh) return;

	if (!Mesh->IsSimulatingPhysics())
	{
		Mesh->SetSimulatePhysics(true);
	}

	// Normalize to avoid scaling issues
	const FVector Dir = ThrowDirection.GetSafeNormal();

	// ---- Linear impulse (trajectory) ----
	const FVector LinearImpulse =
		Dir * ThrowStrength +
		FVector::UpVector * ArcBoost;

	Mesh->AddImpulse(LinearImpulse, NAME_None, true);

	// ---- Angular impulse (spin) ----
	const FVector AngularImpulse =
		FVector(
			FMath::RandRange(-1.f, 1.f),
			FMath::RandRange(-1.f, 1.f),
			FMath::RandRange(-1.f, 1.f)
		) * SpinStrength;

	Mesh->AddAngularImpulseInRadians(AngularImpulse, NAME_None, true);
}


void AFlashbangActor::DoFlashHitbox()
{
	const FVector Center = GetActorLocation();
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes = {
		UEngineTypes::ConvertToObjectType(ECC_Pawn)  // pawns only
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

	for (AActor* A : Hits)
	{
		ApplyStun(A);
		UE_LOG(LogTemp, Log, TEXT("[Flashbang] Overlap: %s"), *A->GetName());
	}
}

void AFlashbangActor::ApplyStun(AActor* A) const
{
	static const FName BB_IsStunned("IsStunned");

	APawn* Pawn = Cast<APawn>(A);
	if (!Pawn) return;

	AAIController* AIC = Cast<AAIController>(Pawn->GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsBool(BB_IsStunned, true);

	// auto-clear after StunDuration
	if (StunDuration > 0.f)
	{
		TWeakObjectPtr<UBlackboardComponent> WeakBB = BB;
		FTimerHandle ClearHandle;
		GetWorld()->GetTimerManager().SetTimer(
			ClearHandle,
			[WeakBB]()
			{
				if (WeakBB.IsValid())
				{
					WeakBB->SetValueAsBool(TEXT("IsStunned"), false);
				}
			},
			StunDuration,
			false
		);
	}
}
