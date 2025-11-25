// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKitUseBehavior.h"
#include "AMazeCharacter.h"

bool UMedKitUseBehavior::Use_Implementation(AAMazeCharacter* User, UItemDef* /*ItemDef*/)
{
	if (!User)
	{
		return false;
	}
	
	{
		User->AddHealth(20);
		return true;
	}
	


}