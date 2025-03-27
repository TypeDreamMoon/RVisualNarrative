#pragma once
#include "EdGraphUtilities.h"

class SGraphNode;

struct FRVNGraphNodeFactory : FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override;
};

struct FRVNStatePinFactory : FGraphPanelPinFactory
{
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* InPin) const override;
};
