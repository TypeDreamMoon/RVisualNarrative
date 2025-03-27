#pragma once

#include "CoreMinimal.h"

class FRVNBlueprintToolBar final
{
public:
	FRVNBlueprintToolBar();

public:
	void Initialize();

	void Deinitialize();

private:
	void BuildAction();

	TSharedRef<FExtender> GenerateBlueprintExtender(UBlueprint* InBlueprint);

private:
	TSharedRef<FUICommandList> CommandList;

	TWeakObjectPtr<UBlueprint> Blueprint;
};
