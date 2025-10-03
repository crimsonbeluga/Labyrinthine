// Fill out your copyright notice in the Description page of Project Settings.


#include "AEnemyGuardCharacter.h"

// Sets default values
AAEnemyGuardCharacter::AAEnemyGuardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAEnemyGuardCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAEnemyGuardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAEnemyGuardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

