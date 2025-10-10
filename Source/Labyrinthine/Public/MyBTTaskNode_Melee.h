// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTaskNode_Melee.generated.h"



class AAMazeCharacter; // forward declare — no include needed

UCLASS()
class LABYRINTHINE_API UMyBTTaskNode_Melee : public UBTTaskNode
{
    GENERATED_BODY()



public:

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; // letting the compiler know this function can be overiden 
    // any subclass that can make its own version of the function



private:
   

    float DamageAmount = 20;
};
