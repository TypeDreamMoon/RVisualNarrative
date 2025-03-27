#pragma once

#include "CoreMinimal.h"
#include "Decorator/Task/RVNSyncTask.h"
#include "RVNSyncTask_Test.generated.h"

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNSyncTask_Test : public URVNSyncTask
{
	GENERATED_BODY()

public:
	virtual void OnStart_Implementation(URVNComponent* OwnerComponent) override;

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent) override;

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent) override;
};
