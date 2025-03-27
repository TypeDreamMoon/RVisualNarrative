#pragma once

#include "CoreMinimal.h"
#include "RVNTask.h"
#include "RVNSyncTask.generated.h"

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNSyncTask : public URVNTaskBase
{
	GENERATED_BODY()

public:
	virtual void OnStart_Implementation(URVNComponent* OwnerComponent) override;

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent) override;

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent) override;
};
