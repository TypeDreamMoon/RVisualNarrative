#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RVNAssetActionsFactory.generated.h"

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNAssetActionsFactory : public UFactory
{
	GENERATED_BODY()

public:
	URVNAssetActionsFactory();
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;

public:
	UClass* ParentClass;
};
