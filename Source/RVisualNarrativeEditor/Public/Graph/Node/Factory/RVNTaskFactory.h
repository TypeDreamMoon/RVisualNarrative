#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RVNTaskFactory.generated.h"

class URVNAsyncTask;
class URVNTaskBase;

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNTaskFactory : public UFactory
{
	GENERATED_BODY()

public:
	URVNTaskFactory();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBlueprintType> BlueprintType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URVNTaskBase> ParentClass;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	//~ Begin UFactory Interface
};
