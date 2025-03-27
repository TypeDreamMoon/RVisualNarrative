#pragma once

#include "CoreMinimal.h"
#include "RVNComponent.h"
#include "EdGraph/EdGraph.h"
#include "Graph/Node/RVNStateNode.h"
#include "RVNDialogueGraph.generated.h"

class URVNComponent;
class FRVNEditor;
class URVNStateNode;
struct FRVNNodeData;

USTRUCT()
struct FRVNPasteNodeData
{
	GENERATED_BODY()

	UPROPERTY()
	FString StateName;

	UPROPERTY()
	FString StateContent;

	UPROPERTY()
	bool bIsPlayer;

	UPROPERTY()
	TArray<FRVNClassInfo> ConditionNode;

	UPROPERTY()
	TArray<FRVNClassInfo> TaskNode;

	UPROPERTY()
	ENodeType NodeType;

	UPROPERTY()
	FVector2D Position;
};

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNDialogueGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	URVNDialogueGraph();

	URVNStateNode* CreateStateNode(ENodeType InType, const FVector2D& InPosition);

	void ProcessPasteNodes(const TArray<URVNStateNode*>& InStateNodes);

	bool GetNodeData(int32 InNodeId, FRVNNodeData& OutData) const;

	void OnStateNodeIdChanged(TMap<int32, int32> InIdMap);

	void OnPinConnectionChanged(int32 InNodeId, const TArray<int32>& NextNodesId);

	void OnSpeakerNameChanged(int32 InNodeId, const FString& InNewName) const;

	void OnDialogueContentChanged(int32 InNodeId, const FString& InNewContent) const;

	void OnIsPlayerChanged(int32 InNodeId, bool bIsPlayer) const;

	void OnPositionChanged(int32 InNodeId, const FVector2D& InNewPosition);

	void OnPropertyChanged(const FPropertyChangedEvent& InPropertyChangedEvent);

	TSharedPtr<SGraphNode> GetNodeWidgetFromGuid(const FGuid InGuid) const;

	FORCEINLINE void RemoveStateNode(int32 InNodeId) const;

	FORCEINLINE void AddCondition(int32 NodeId, const FRVNClassInfo& Condition) const;

	FORCEINLINE void RemoveCondition(int32 NodeId, const FRVNClassInfo& Condition) const;

	FORCEINLINE void AddTask(int32 NodeId, const FRVNClassInfo& Task) const;

	FORCEINLINE void RemoveTask(int32 NodeId, const FRVNClassInfo& Task) const;

public:
	UPROPERTY()
	TObjectPtr<URVNComponent> RVNCompPtr;

	TWeakPtr<FRVNEditor> RVNEditorPtr;

	UPROPERTY()
	TObjectPtr<UEdGraphNode> Entry;

	UPROPERTY()
	TMap<int32, URVNStateNode*> IdToNodeMap;

private:
	FTimerHandle ThrottleTimerHandle;
	bool bIsTimerActive = false;
};
