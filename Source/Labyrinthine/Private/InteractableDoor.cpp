// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableDoor.h"

// Sets default values
AInteractableDoor::AInteractableDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(DoorFrameMesh);

	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame Mesh"));
	SetRootComponent(DoorFrameMesh);






}


// Called when the game starts or when spawned
void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

 void AInteractableDoor::Interact_Implementation(AActor* Interactor)
{

}

 FText AInteractableDoor::GetPromptText_Implementation() const 
 {
	 return FText::GetEmpty();
 }

 void AInteractableDoor::OpenDoor(float DeltaTime)
 {

 }