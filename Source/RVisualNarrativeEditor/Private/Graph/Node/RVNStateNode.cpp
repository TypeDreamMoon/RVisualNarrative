#include "Graph/Node/RVNStateNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Styling/SlateTypes.h"
#include "RVNComponent.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"
#include "Graph/RVNDialogueGraph.h"

URVNStateNode::URVNStateNode()
{
}

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

void URVNStateNode::SetDialogText(const FString& NewText)
{
	StateContent = NewText;

	OnDialogueContentChanged();
}

void URVNStateNode::PasteDecorator(const TArray<URVNDecorator*>& InDecorator)
{
	for (const auto Decorator : InDecorator)
	{
		PasteDecorator(Decorator);
	}
}

void URVNStateNode::PasteDecorator(const URVNDecorator* InDecorator)
{
	AddDecorator(RVNDialogueGraph->CreateDecorator(InDecorator->GetClass()));
}

void URVNStateNode::OnSelectedDecorator(URVNDecorator* InDecorator)
{
	if (InDecorator == nullptr)
	{
		return;
	}

	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

	GetDialogueGraph()->SetSelectedDecorator(InDecorator);
}

void URVNStateNode::AddDecorator(URVNDecorator* InDecorator)
{
	if (InDecorator == nullptr)
	{
		return;
	}

	Modify();

	if (auto Condition = Cast<URVNConditionBase>(InDecorator))
	{
		AddCondition(Condition);
	}
	else if (auto Task = Cast<URVNTaskBase>(InDecorator))
	{
		AddTask(Task);
	}
}

void URVNStateNode::AddCondition(URVNConditionBase* InCondition)
{
	if (InCondition == nullptr)
	{
		return;
	}

	if (ConditionNodes.Contains(InCondition))
	{
		return;
	}

	if (NodeType == ENodeType::ENT_Entry)
	{
		return;
	}

	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

	ConditionNodes.Add(InCondition);

	GetDialogueGraph()->AddCondition(NodeId, InCondition);

	OnAddConditionCallback.ExecuteIfBound(InCondition);
}

void URVNStateNode::AddTask(URVNTaskBase* InTask)
{
	if (InTask == nullptr)
	{
		return;
	}

	if (TaskNodes.Contains(InTask))
	{
		return;
	}

	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

	TaskNodes.Add(InTask);

	GetDialogueGraph()->AddTask(NodeId, InTask);

	OnAddTaskCallback.ExecuteIfBound(InTask);
}

void URVNStateNode::RemoveDecorator(URVNDecorator* InDecorator)
{
	if (InDecorator == nullptr)
	{
		return;
	}

	Modify();

	if (auto Condition = Cast<URVNConditionBase>(InDecorator))
	{
		RemoveCondition(Condition);
	}
	else if (auto Task = Cast<URVNTaskBase>(InDecorator))
	{
		RemoveTask(Task);
	}
}

void URVNStateNode::RemoveCondition(URVNConditionBase* InCondition)
{
	if (InCondition == nullptr)
	{
		return;
	}

	if (!ConditionNodes.Remove(InCondition))
	{
		return;
	}

	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

	GetDialogueGraph()->RemoveCondition(NodeId, InCondition);

	OnRemoveConditionCallback.ExecuteIfBound(InCondition);
}

void URVNStateNode::RemoveTask(URVNTaskBase* InTask)
{
	if (InTask == nullptr)
	{
		return;
	}

	if (!TaskNodes.Remove(InTask))
	{
		return;
	}

	GetDialogueGraph()->RemoveTask(NodeId, InTask);

	OnRemoveTaskCallback.ExecuteIfBound(InTask);
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

		OnNodeIdChangedCallback.ExecuteIfBound();
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

	if (GetDialogueGraph() == nullptr)
	{
		return;
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
	if (GetDialogueGraphConst() == nullptr)
	{
		return;
	}

	GetDialogueGraphConst()->OnSpeakerNameChanged(GetNodeId(), StateName);
}

void URVNStateNode::OnDialogueContentChanged() const
{
	if (GetDialogueGraphConst() == nullptr)
	{
		return;
	}

	GetDialogueGraphConst()->OnDialogueContentChanged(GetNodeId(), StateContent);
}

void URVNStateNode::OnIsPlayerChanged() const
{
	if (GetDialogueGraphConst() == nullptr)
	{
		return;
	}

	GetDialogueGraphConst()->OnIsPlayerChanged(GetNodeId(), bIsPlayer);
}

void URVNStateNode::OnPositionChanged()
{
	if (GetDialogueGraph() == nullptr)
	{
		return;
	}

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
