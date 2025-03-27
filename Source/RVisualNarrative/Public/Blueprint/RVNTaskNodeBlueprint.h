#pragma once

#include "CoreMinimal.h"
#include "RVNDecoratorBlueprint.h"
#include "Engine/Blueprint.h"
#include "RVNTaskNodeBlueprint.generated.h"

UCLASS()
class RVISUALNARRATIVE_API URVNTaskNodeBlueprint : public URVNDecoratorBlueprint
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

	static URVNTaskNodeBlueprint* FindRootRVNNodeBlueprint(const URVNTaskNodeBlueprint* DerivedBlueprint);

	virtual UClass* GetParentClass() override;
#endif
};
