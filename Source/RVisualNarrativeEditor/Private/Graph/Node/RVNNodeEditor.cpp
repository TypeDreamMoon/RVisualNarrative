#include "Graph/Node/RVNNodeEditor.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Blueprint/RVNConditionNodeBlueprint.h"
#include "Blueprint/RVNTaskNodeBlueprint.h"

#define LOCTEXT_NAMESPACE "RVNNodeEditor"

FRVNNodeEditor::FRVNNodeEditor()
{
}

FRVNNodeEditor::~FRVNNodeEditor()
{
}

void FRVNNodeEditor::InitNodeEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
                                    const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);

	for (auto Blueprint : InBlueprints)
	{
		EnsureGameplayAbilityBlueprintIsUpToDate(Blueprint);
	}
}

void FRVNNodeEditor::EnsureGameplayAbilityBlueprintIsUpToDate(UBlueprint* Blueprint)
{
#if WITH_EDITORONLY_DATA
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		if (Graph->GetName() == "EventGraph" && Graph->Nodes.Num() == 0)
		{
			if (!Graph->Schema->GetClass()->IsChildOf(UEdGraphSchema_K2::StaticClass()))
			{
				continue;
			}

			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);

			break;
		}
	}
#endif
}

FName FRVNNodeEditor::GetToolkitFName() const
{
	return FName("RVNNodeEditor");
}

FText FRVNNodeEditor::GetBaseToolkitName() const
{
	return LOCTEXT("RVNNodeEditorAppLabel", "RVB Node Editor");
}

FText FRVNNodeEditor::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	if (EditingObjs.Num() <= 0)
	{
		return FText();
	}

	FFormatNamedArguments Args;

	const UObject* EditingObject = EditingObjs[0];

	const bool bDirtyState = EditingObject->GetOutermost()->IsDirty();

	Args.Add(TEXT("ObjectName"), FText::FromString(EditingObject->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());

	return FText::Format(LOCTEXT("GameplayAbilitiesToolkitName", "{ObjectName}{DirtyState}"), Args);
}

FText FRVNNodeEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetEditingObject();

	if (EditingObject == nullptr)
	{
		return FText();
	}

	return GetToolTipTextForObject(EditingObject);
}

FString FRVNNodeEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("RVNNodeEditor");
}

FLinearColor FRVNNodeEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FRVNNodeEditor::GetDocumentationLink() const
{
	return FBlueprintEditor::GetDocumentationLink();
}

UBlueprint* FRVNNodeEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<URVNTaskNodeBlueprint>())
		{
			return Cast<UBlueprint>(EditingObjs[i]);
		}

		if (EditingObjs[i]->IsA<URVNConditionNodeBlueprint>())
		{
			return Cast<UBlueprint>(EditingObjs[i]);
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
