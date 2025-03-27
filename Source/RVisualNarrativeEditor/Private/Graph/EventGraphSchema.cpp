#include "Graph/EventGraphSchema.h"

#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_Event.h"

void URVNEventGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	TArray<FName> EventFuncName = {
		"ReceiveBeginPlay",
		"ReceiveTick"
	};

	if (Graph.GetOuter() == nullptr)
	{
		return;
	}

	const auto OverrideFuncClass = Cast<UBlueprint>(Graph.GetOuter())->ParentClass;

	for (int Index = 0; Index < EventFuncName.Num(); Index++)
	{
		if (FindUField<UFunction>(OverrideFuncClass, EventFuncName[Index]) == nullptr)
		{
			continue;
		}

		const auto SpawnPos = Graph.GetGoodPlaceForNewNode();

		FName EventName = EventFuncName[Index];

		FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_Event>(
			&Graph,
			SpawnPos,
			EK2NewNodeFlags::SelectNewNode,
			[EventName, OverrideFuncClass](UK2Node_Event* NewInstance)
			{
				NewInstance->EventReference.SetExternalMember(EventName, OverrideFuncClass);
				NewInstance->bOverrideFunction = true;
			}
		);
	}
}
