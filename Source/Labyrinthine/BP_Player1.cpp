// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Player1.h"

// Sets default values
ABP_Player1::ABP_Player1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.



	  

}

// Called when the game starts or when spawned
void ABP_Player1::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_Player1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABP_Player1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

