#include "FlashbangUseBehavior.h"
#include "FlashbangActor.h"
#include "AMazeCharacter.h"
#include "ItemDef.h"
#include "Kismet/GameplayStatics.h"

bool UFlashbangUseBehavior::Use_Implementation(AAMazeCharacter* User, UItemDef* /*ItemDef*/)
{
	if (!User || !FlashbangClass) return false;

	if (PinPullSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(User, PinPullSFX, User->GetActorLocation());
	}

	const FVector spawnLoc = User->GetActorLocation() + User->GetActorForwardVector() * SpawnForwardOffset;
	const FRotator spawnRot = FRotator::ZeroRotator;

	FActorSpawnParameters p;
	p.Owner = User;
	p.Instigator = User;
	p.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AFlashbangActor* fb = User->GetWorld()->SpawnActor<AFlashbangActor>(FlashbangClass, spawnLoc, spawnRot, p);
	if (!fb) return false;



	


	if (DropSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(User, DropSFX, spawnLoc);
	}

	FRotator ControlRot = User->GetControlRotation();
	FVector ThrowDir = ControlRot.Vector();
	

	fb->Throw(ThrowDir);

	fb->Arm(FuseSeconds, User);
	return true;
}
