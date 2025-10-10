#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTaskNode_Attack.generated.h"

UCLASS()
class LABYRINTHINE_API UMyBTTaskNode_Attack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UMyBTTaskNode_Attack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
    // Blackboard key that holds the current target (your player)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

    // How much damage to apply on attack
    UPROPERTY(EditAnywhere, Category = "Attack")
    float DamageAmount = 20.f;
};
