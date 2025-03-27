#include "RVNAssetActionsFactory.h"
#include "RVNAssetBlueprint.h"
#include "RVNComponent.h"
#include "Kismet2/KismetEditorUtilities.h"

URVNAssetActionsFactory::URVNAssetActionsFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = URVNAssetBlueprint::StaticClass();
	ParentClass = UActorComponent::StaticClass();
}

bool URVNAssetActionsFactory::ConfigureProperties()
{
	return true;
}

UObject* URVNAssetActionsFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                   UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn);
}

UObject* URVNAssetActionsFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                   UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(URVNAssetBlueprint::StaticClass()));

	//const auto NewBP = FKismetEditorUtilities::CreateBlueprint(URVNComponent::StaticClass(), InParent, InName, BPTYPE_Normal, InClass, UBlueprintGeneratedClass::StaticClass());

	const auto NewBP = NewObject<URVNAssetBlueprint>(InParent, InClass, InName, Flags);

	NewBP->ParentClass = URVNComponent::StaticClass();
	NewBP->BlueprintType = BPTYPE_Normal;
	NewBP->bIsNewlyCreated = true;
	NewBP->bLegacyNeedToPurgeSkelRefs = false;
	NewBP->GenerateNewGuid();

	return NewBP;
}
