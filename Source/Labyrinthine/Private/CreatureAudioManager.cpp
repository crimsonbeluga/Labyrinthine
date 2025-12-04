// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureAudioManager.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCreatureAudioManager::UCreatureAudioManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCreatureAudioManager::BeginPlay()
{
	Super::BeginPlay();

	ComputeRandomInterval();
	
}


// Called every frame
void UCreatureAudioManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreatureAudioManager::ComputeRandomInterval()
{
	float randomAudioInterval = FMath::FRandRange(MinAudioInterval, MaxAudioInterval);


	
}
