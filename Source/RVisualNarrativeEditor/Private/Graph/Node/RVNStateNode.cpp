#include "Graph/Node/RVNStateNode.h"

#include "RVNComponent.h"
#include "Graph/RVNDialogueGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Graph/RVNDialogueGraph.h"
#include "Styling/SlateTypes.h"

void URVNStateNode::InitStateNode(FRVNNodeData& InData, ENodeType InType, TObjectPtr<URVNDialogueGraph> InGraph)
{
	NodeId = InData.NodeId;
	bIsPlayer = InData.bIsPlayer;
	StateName = InData.StateName;
	StateContent = InData.StateContent;
	NodeType = InType;

	RVNDialogueGraph = InGraph;

	switch (InType)
	{
	case ENodeType::ENT_Entry:
		{
			StateName = InData.StateName = TEXT("Root");
			StateContent = InData.StateContent = TEXT("You Can't Edit Root");
			break;
		}

	case ENodeType::ENT_Select:
		{
			StateName = InData.StateName = TEXT("Select");
			StateContent = InData.StateContent = TEXT("Selector Name");
			break;
		}

	default:
		{
			break;
		}
	}
}

TSharedPtr<SGraphNode> URVNStateNode::GetStateWidget() const
{
	return RVNDialogueGraph->GetNodeWidgetFromGuid(NodeGuid);
}

FRVNNodeData URVNStateNode::GetPastedData() const
{
	FRVNNodeData Data;
	Data.StateName = StateName;
	Data.StateContent = StateContent;
	//Data.bIsPlayer = bIsPlayer;
	Data.Conditions = ConditionNode;
	Data.Tasks = TaskNode;
	//Data.bIsSelector = IsSelectNode();

	return Data;
}

void URVNStateNode::SetDialogText(const FString& NewText)
{
	StateContent = NewText;

	OnDialogueContentChanged();
}

void URVNStateNode::AddCondition(const FRVNClassInfo& InNode)
{
	if (ConditionNode.Contains(InNode))
	{
		return;
	}

	if (NodeType == ENodeType::ENT_Entry)
	{
		return;
	}

	if (InNode.ObjectPath.IsEmpty())
	{
		return;
	}

	ConditionNode.Add(InNode);

	GetDialogueGraph()->AddCondition(NodeId, InNode);

	OnAddCondition.ExecuteIfBound(InNode);
}

void URVNStateNode::RemoveCondition(const FRVNClassInfo& InNode)
{
	if (!ConditionNode.Remove(InNode))
	{
		return;
	}

	if (InNode.ObjectPath.IsEmpty())
	{
		return;
	}

	GetDialogueGraph()->RemoveCondition(NodeId, InNode);

	OnRemoveCondition.ExecuteIfBound(InNode);
}

void URVNStateNode::AddTask(const FRVNClassInfo& InNode)
{
	if (TaskNode.Contains(InNode))
	{
		return;
	}

	if (InNode.ObjectPath.IsEmpty())
	{
		return;
	}

	TaskNode.Add(InNode);

	GetDialogueGraph()->AddTask(NodeId, InNode);

	OnAddTask.ExecuteIfBound(InNode);
}

void URVNStateNode::RemoveTask(const FRVNClassInfo& InNode)
{
	if (!TaskNode.Remove(InNode))
	{
		return;
	}

	if (InNode.ObjectPath.IsEmpty())
	{
		return;
	}

	GetDialogueGraph()->RemoveTask(NodeId, InNode);

	OnRemoveTask.ExecuteIfBound(InNode);
}

int32 URVNStateNode::GetNodeId() const
{
	return NodeId;
}

void URVNStateNode::SetNodeId(const int32 NewId)
{
	if (NodeType != ENodeType::ENT_Entry)
	{
		NodeId = NewId;

		OnNodeIdChanged.ExecuteIfBound();
	}
}

void URVNStateNode::AllocateDefaultPins()
{
	// 创建输入输出引脚  
	if (!IsEntryNode())
	{
		CreatePin(EGPD_Input, "StatePin", "In");
	}

	CreatePin(EGPD_Output, "StatePin", "Out");
}

void URVNStateNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}

	Super::AutowireNewNode(FromPin);
}

FText URVNStateNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(StateName);
}

void URVNStateNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	TArray<int32> NextNodesId;

	// 遍历输出引脚的连接  
	if (UEdGraphPin* OutputPin = FindPin(TEXT("Out")))
	{
		for (UEdGraphPin* LinkedTo : OutputPin->LinkedTo)
		{
			if (URVNStateNode* StateNode = Cast<URVNStateNode>(LinkedTo->GetOwningNode()))
			{
				NextNodesId.AddUnique(StateNode->GetNodeId());
			}
		}
	}

	GetDialogueGraph()->OnPinConnectionChanged(GetNodeId(), NextNodesId);
}

UEdGraphPin* URVNStateNode::GetInputPin() const
{
	return FindPin(TEXT("In"));
}

UEdGraphPin* URVNStateNode::GetOutputPin() const
{
	return FindPin(TEXT("Out"));
}

void URVNStateNode::OnPropertyEdited(const FPropertyChangedEvent& InPropertyChangedEvent) const
{
	const FName PropertyName = InPropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URVNStateNode, StateName))
	{
		OnSpeakerNameChanged();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URVNStateNode, StateContent))
	{
		OnDialogueContentChanged();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URVNStateNode, bIsPlayer))
	{
		OnIsPlayerChanged();
	}
}

void URVNStateNode::OnSpeakerNameChanged() const
{
	GetDialogueGraphConst()->OnSpeakerNameChanged(GetNodeId(), StateName);
}

void URVNStateNode::OnDialogueContentChanged() const
{
	GetDialogueGraphConst()->OnDialogueContentChanged(GetNodeId(), StateContent);
}

void URVNStateNode::OnIsPlayerChanged() const
{
	GetDialogueGraphConst()->OnIsPlayerChanged(GetNodeId(), bIsPlayer);
}

void URVNStateNode::OnPositionChanged()
{
	GetDialogueGraph()->OnPositionChanged(GetNodeId(), FVector2d(NodePosX, NodePosY));
}

const URVNDialogueGraph* URVNStateNode::GetDialogueGraphConst() const
{
	if (GetGraph() == nullptr)
	{
		return nullptr;
	}

	return Cast<URVNDialogueGraph>(GetGraph());
}

URVNDialogueGraph* URVNStateNode::GetDialogueGraph()
{
	if (GetGraph() == nullptr)
	{
		return nullptr;
	}

	if (!RVNDialogueGraph.IsValid())
	{
		RVNDialogueGraph = Cast<URVNDialogueGraph>(GetGraph());
	}

	return RVNDialogueGraph.Get();
}
