#include "Decorator/DefaultNode/RVNCondition_Default.h"

bool URVNCondition_Default::CheckCondition_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNCondition_Default::CheckCondition()"));

	return 114514 > 0;
}
