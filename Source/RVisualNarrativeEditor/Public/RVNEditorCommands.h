#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "RVNEditorStyle.h"

class FRVNEditorCommands final : public TCommands<FRVNEditorCommands>
{
public:
	FRVNEditorCommands()
		: TCommands<FRVNEditorCommands>(
			TEXT("RVisualNarrativeEditor"),
			NSLOCTEXT("Contexts", "RVisualNarrativeEditor", "RVisualNarrativeEditor Plugin"),
			NAME_None,
			FRVNEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenEditorSettings;

	TSharedPtr<FUICommandInfo> OpenRuntimeSettings;

	TSharedPtr<FUICommandInfo> CompactNodesData;
};
