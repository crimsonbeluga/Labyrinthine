#include "MyBTTaskNode_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AMazeCharacter.h"

UMyBTTaskNode_Attack::UMyBTTaskNode_Attack()
{
    NodeName = TEXT("Attack: Deal Damage to Target");

    // Limit the key picker to Actor/Object types
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UMyBTTaskNode_Attack, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UMyBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    UObject* TargetObj = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
    if (!TargetObj) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(TargetObj);
    if (!TargetActor) return EBTNodeResult::Failed;

    // Cast to your player class
    AAMazeCharacter* PlayerCharacter = Cast<AAMazeCharacter>(TargetActor);
    if (!PlayerCharacter) return EBTNodeResult::Failed;

    // Apply damage via your C++ function
    PlayerCharacter->DealDamage(DamageAmount);

    return EBTNodeResult::Succeeded;
}
