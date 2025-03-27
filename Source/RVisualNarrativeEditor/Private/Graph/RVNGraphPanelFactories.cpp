#include "Graph/RVNGraphPanelFactories.h"
#include "EdGraph/EdGraphNode.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/Slate/SRVNStateNode.h"
#include "Graph/Node/Slate/SRVNStatePin.h"

TSharedPtr<SGraphNode> FRVNGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (URVNStateNode* StateNode = Cast<URVNStateNode>(InNode))
	{
		return SNew(SRVNStateWidget, StateNode);
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FRVNStatePinFactory::CreatePin(UEdGraphPin* InPin) const
{
	return SNew(SRVNStatePin, InPin);
}
