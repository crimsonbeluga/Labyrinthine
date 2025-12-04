// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreatureAudioManager.generated.h"

 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LABYRINTHINE_API UCreatureAudioManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCreatureAudioManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ComputeRandomInterval();

	FTimerHandle AudioRepeatTimer;

	float MinAudioInterval = 5.f;

	float MaxAudioInterval = 60.f;

	





};
