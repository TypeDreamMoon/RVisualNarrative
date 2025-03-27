#include "Blueprint/RVNConditionNodeBlueprint.h"
#include "Decorator/Condition/RVNCondition.h"

#if WITH_EDITOR
URVNConditionNodeBlueprint* URVNConditionNodeBlueprint::FindRootRVNNodeBlueprint(
	const URVNConditionNodeBlueprint* DerivedBlueprint)
{
	URVNConditionNodeBlueprint* ParentBP = nullptr;

	for (const UClass* ParentClass = DerivedBlueprint->ParentClass; ParentClass != UObject::StaticClass(); ParentClass = ParentClass->GetSuperClass())
	{
		if (URVNConditionNodeBlueprint* TestBP = Cast<URVNConditionNodeBlueprint>(ParentClass->ClassGeneratedBy))
		{
			ParentBP = TestBP;
		}
	}

	return ParentBP;
}

UClass* URVNConditionNodeBlueprint::GetParentClass()
{
	return URVNConditionBase::StaticClass();
}
#endif
