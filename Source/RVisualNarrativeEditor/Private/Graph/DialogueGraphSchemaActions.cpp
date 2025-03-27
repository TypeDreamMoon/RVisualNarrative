#include "Graph/DialogueGraphSchemaActions.h"

#include "Graph/RVNDialogueGraph.h"
#include "Graph/Node/RVNStateNode.h"

UEdGraphNode* FDGSchemaStateNodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                      const FVector2D Location, bool bSelectNewNode)
{
	URVNDialogueGraph* CurrentGraph = CastChecked<URVNDialogueGraph>(ParentGraph);

	if (CurrentGraph == nullptr)
	{
		return nullptr;
	}

	const auto NewStateNode = CurrentGraph->CreateStateNode(ENodeType::ENT_Dialogue, Location);

	NewStateNode->NodePosX = Location.X;
	NewStateNode->NodePosY = Location.Y;
	NewStateNode->AutowireNewNode(FromPin);

	ParentGraph->Modify();
	ParentGraph->bEditable = true;
	ParentGraph->bAllowRenaming = true;

	ParentGraph->NotifyGraphChanged();

	return NewStateNode;
}

UEdGraphNode* FDGSchemaSelectorNodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                         const FVector2D Location, bool bSelectNewNode)
{
	URVNDialogueGraph* CurrentGraph = CastChecked<URVNDialogueGraph>(ParentGraph);

	if (CurrentGraph == nullptr)
	{
		return nullptr;
	}

	const auto NewStateNode = CurrentGraph->CreateStateNode(ENodeType::ENT_Select, Location);

	NewStateNode->NodePosX = Location.X;
	NewStateNode->NodePosY = Location.Y;
	NewStateNode->AutowireNewNode(FromPin);

	ParentGraph->Modify();
	ParentGraph->bEditable = true;
	ParentGraph->bAllowRenaming = true;

	ParentGraph->NotifyGraphChanged();

	return NewStateNode;
}
