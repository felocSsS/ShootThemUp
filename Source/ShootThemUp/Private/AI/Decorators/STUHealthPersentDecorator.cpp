// Shoot Them Up Game. All Rights Reserved.


#include "AI/Decorators/STUHealthPersentDecorator.h"
#include "AIController.h"
#include "STUUtils.h"
#include "Components/STUHealthComponent.h"

USTUHealthPersentDecorator::USTUHealthPersentDecorator()
{
    NodeName = "Health Percent";
}

bool USTUHealthPersentDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const auto Controller = OwnerComp.GetAIOwner();
    if (!Controller) return false;

    const auto HelathComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(Controller->GetPawn());
    if (!HelathComponent || HelathComponent->IsDead()) return false;

    return HelathComponent->GetHealthPersent() <= HealthPercent;
}
