#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RVNDecorator.generated.h"

class URVNComponent;

UCLASS()
class RVISUALNARRATIVE_API URVNDecorator : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FString GetNodeIconName() const;
#endif
};
