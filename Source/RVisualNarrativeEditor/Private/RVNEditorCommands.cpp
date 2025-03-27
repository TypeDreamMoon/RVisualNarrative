#include "RVNEditorCommands.h"

#define LOCTEXT_NAMESPACE "FRVNEditorCommands"

void FRVNEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenEditorSettings, "Open Editor Settings", "Open RVNEditor Settings Action",
			   EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(OpenRuntimeSettings, "Open Runtime Settings", "Open RVNRuntime Settings Action",
			   EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CompactNodesData, "Compact Nodes Data", "Compact RVNComponent Nodes Data",
			   EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE