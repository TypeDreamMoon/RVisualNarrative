#pragma once

#include "CoreMinimal.h"
#include "RVNDecoratorBlueprint.h"
#include "RVNConditionNodeBlueprint.generated.h"

UCLASS()
class RVISUALNARRATIVE_API URVNConditionNodeBlueprint : public URVNDecoratorBlueprint
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override
	{
		return false;
	}

	// End of UBlueprint interface

	static URVNConditionNodeBlueprint* FindRootRVNNodeBlueprint(const URVNConditionNodeBlueprint* DerivedBlueprint);

	virtual UClass* GetParentClass() override;
#endif
};
