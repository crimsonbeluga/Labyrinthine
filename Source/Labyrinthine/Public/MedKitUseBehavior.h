// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemUseBehavior.h"
#include "MedKitUseBehavior.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHINE_API UMedKitUseBehavior : public UItemUseBehavior
{
	GENERATED_BODY()
	

public:
	
	


	// NOTE: override the _Implementation form
	virtual bool Use_Implementation(AAMazeCharacter* User, UItemDef* ItemDef) override;

};
