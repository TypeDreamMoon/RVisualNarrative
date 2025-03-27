#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "RVNDecoratorBlueprint.generated.h"

UINTERFACE(MinimalAPI)
class URVNDecoratorInterface : public UInterface
{
	GENERATED_BODY()
};

class RVISUALNARRATIVE_API IRVNDecoratorInterface
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual UClass* GetParentClass() = 0;
#endif
};

UCLASS(Abstract)
class RVISUALNARRATIVE_API URVNDecoratorBlueprint : public UBlueprint, public IRVNDecoratorInterface
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override;
	// End of UBlueprint interface

	virtual UClass* GetParentClass() override;

#endif
};
