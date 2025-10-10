// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode_Melee.h"
#include "AMazeCharacter.h" // included so we can access the deal damage function and pass in the damage amount.
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UMyBTTaskNode_Melee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)  
//"EBTNodeResult" is a name space we can access because of are includes. inside that name space there is a Enum called type were getting that cause its gonna be used here.
// "UMyBTTaskNode_Melee" is the class name that the function were calling "ExecuteTask" belongs to
// the "UBehaviorTreeComponent"  reference is handed to you when the task executes. it knows what behavior tree its being executed on.
// the "NodeMemory" pointer is a raw byte buffer allocated by the Behavior Tree runtime for each task instance. It acts as small, fast storage that exists only while the node is active.
// //You use it when your task needs to remember information across multiple ticks, such as storing a start time, a cached target, 
// or tracking partial progress while the task continues running.
{

	auto* BB = OwnerComp.GetBlackboardComponent();
	//auto just tells the compiler to automatically figure out the variable’s type based on whatever value you’re assigning to it. in this case a UBlackboard component
	 // were calling  the behavioir tree from owner component and telling it to get the black baord assigneed to it and set that as a pointer called BB


	auto* Player = Cast<AAMazeCharacter>(BB->GetValueAsObject("TargetActor"));

	// from the black board find return the target actor as a object and attempt to cast it as a type of AAMazeCharacter if it works then store that in a pointer called player

	if (Player) //checking if we 
	{
		Player->DealDamage(DamageAmount);

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;

	}

}