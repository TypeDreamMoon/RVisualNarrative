#pragma once

#include "CoreMinimal.h"

class RVISUALNARRATIVECORE_API FRVisualNarrativeCoreModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static FRVisualNarrativeCoreModule& Get();
};
