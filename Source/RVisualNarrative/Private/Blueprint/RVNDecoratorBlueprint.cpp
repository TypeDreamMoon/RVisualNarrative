#include "Blueprint/RVNDecoratorBlueprint.h"

#if WITH_EDITOR
bool URVNDecoratorBlueprint::SupportedByDefaultBlueprintFactory() const
{
	return false;
}

UClass* URVNDecoratorBlueprint::GetParentClass()
{
	return nullptr;
}
#endif