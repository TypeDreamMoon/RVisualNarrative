#pragma once

#include "CoreMinimal.h"
#include "Decorator/RVNDecorator.h"
#include "RVNCondition.generated.h"

struct FRVNConditionParams
{
	bool bIsPass = false;
	URVNComponent* InRVNComponent = nullptr;
};

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNConditionBase : public URVNDecorator
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual FString GetNodeIconName() const override;
#endif

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Condition")
	bool CheckCondition(URVNComponent* OwnerComponent);
	virtual bool CheckCondition_Implementation(URVNComponent* OwnerComponent);
};

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNCondition : public URVNConditionBase
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition_Implementation(URVNComponent* OwnerComponent) override;
};
