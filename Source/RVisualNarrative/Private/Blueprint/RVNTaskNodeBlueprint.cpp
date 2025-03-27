#include "Blueprint/RVNTaskNodeBlueprint.h"
#include "Decorator/Task/RVNTask.h"

#if WITH_EDITOR
URVNTaskNodeBlueprint* URVNTaskNodeBlueprint::FindRootRVNNodeBlueprint(const URVNTaskNodeBlueprint* DerivedBlueprint)
{
	URVNTaskNodeBlueprint* ParentBP = nullptr;

	for (const UClass* ParentClass = DerivedBlueprint->ParentClass; ParentClass != UObject::StaticClass(); ParentClass =
	     ParentClass->GetSuperClass())
	{
		if (URVNTaskNodeBlueprint* TestBP = Cast<URVNTaskNodeBlueprint>(ParentClass->ClassGeneratedBy))
		{
			ParentBP = TestBP;
		}
	}

	return ParentBP;
}

UClass* URVNTaskNodeBlueprint::GetParentClass()
{
	return URVNTaskBase::StaticClass();
}
#endif
