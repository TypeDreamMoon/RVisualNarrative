#pragma once

#include "CoreMinimal.h"
#include "Decorator/Condition/RVNCondition.h"
#include "RVNCondition_Default.generated.h"

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNCondition_Default : public URVNCondition
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition_Implementation(URVNComponent* OwnerComponent) override;
};
