#include "Decorator/Condition/RVNCondition.h"

#if WITH_EDITOR
FString URVNConditionBase::GetNodeIconName() const
{
	return TEXT("Icons.Help");
}
#endif

bool URVNConditionBase::CheckCondition_Implementation(URVNComponent* OwnerComponent)
{
	return false;
}

bool URVNCondition::CheckCondition_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNConditionBase::BP_CheckCondition_Implementation()!"));

	return false;
}
