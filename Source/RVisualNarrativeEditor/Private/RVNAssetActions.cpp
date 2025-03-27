#include "RVisualNarrativeEditor/Public/RVNAssetActions.h"
#include "RVNAssetBlueprint.h"
#include "RVNComponent.h"
#include "RVNEditor.h"

FRVNAssetActions::FRVNAssetActions(EAssetTypeCategories::Type AsstCategory)
{
	RVNAssetCategory = AsstCategory;
}

FText FRVNAssetActions::GetName() const
{
	return NSLOCTEXT("RVNActions", "RVNActionsName", "RVisual Narrative Component");
}

FColor FRVNAssetActions::GetTypeColor() const
{
	return FColor(255, 175, 0);
}

UClass* FRVNAssetActions::GetSupportedClass() const
{
	return URVNAssetBlueprint::StaticClass();
}

void FRVNAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                       const TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const auto Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (const auto Object : InObjects)
	{
		if (const auto NewBlueprint = Cast<URVNAssetBlueprint>(Object))
		{
			const auto NewEditor = MakeShared<FRVNEditor>();

			NewEditor->InitRVNEditor(Mode, EditWithinLevelEditor, NewBlueprint);
		}
	}
}

uint32 FRVNAssetActions::GetCategories()
{
	return RVNAssetCategory;
}
