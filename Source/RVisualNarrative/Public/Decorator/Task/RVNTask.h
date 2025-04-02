#pragma once

#include "CoreMinimal.h"
#include "Decorator/RVNDecorator.h"
#include "RVNTask.generated.h"

struct FRVNTaskParams
{
	URVNComponent* InRVNComponent = nullptr;
};

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNTaskBase : public URVNDecorator
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FString GetNodeIconName() const override;
#endif

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void OnStart(URVNComponent* OwnerComponent);

	virtual void OnStart_Implementation(URVNComponent* OwnerComponent)
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void OnEnd(URVNComponent* OwnerComponent);

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent)
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void ExecuteTask(URVNComponent* OwnerComponent);

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent)
	{
	}
};
