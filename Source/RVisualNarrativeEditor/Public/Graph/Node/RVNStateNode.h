#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ClassCollector/RVNClassCollector.h"
#include "RVNStateNode.generated.h"

class URVNDialogueGraph;
class SRVNStateWidget;
class URVNConditionNode;
class URVNTaskNode;
struct FRVNNodeData;

UENUM()
enum class ENodeType : uint8
{
	ENT_Entry,
	ENT_Dialogue,
	ENT_Select
};

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNStateNode : public UEdGraphNode
{
	GENERATED_BODY()

	//DECLARE_DELEGATE(FOnPropertyChanged);
	DECLARE_DELEGATE(FOnNodeIdChanged);
	DECLARE_DELEGATE_OneParam(FOnAddCondition, FRVNClassInfo);
	DECLARE_DELEGATE_OneParam(FOnAddTask, FRVNClassInfo);
	DECLARE_DELEGATE_OneParam(FOnRemoveCondition, FRVNClassInfo);
	DECLARE_DELEGATE_OneParam(FOnRemoveTask, FRVNClassInfo);

public:
	URVNStateNode()
	{
	}

	void InitStateNode(FRVNNodeData& InData, ENodeType InType, TObjectPtr<URVNDialogueGraph> InGraph);

	// 数据访问接口
	FORCEINLINE TSharedPtr<SGraphNode> GetStateWidget() const;
	FORCEINLINE FRVNNodeData GetPastedData() const;
	//FORCEINLINE void SetStateWidget(SRVNStateWidget* InWidget) { MySlateNode = InWidget; }
	FORCEINLINE void SetDialogText(const FString& NewText);
	FORCEINLINE FString GetSpeakerName() const { return StateName; }
	FORCEINLINE FString GetDialogText() const { return StateContent; }
	FORCEINLINE bool IsEntryNode() const { return GetNodeType() == ENodeType::ENT_Entry; }
	FORCEINLINE bool IsDialogueNode() const { return GetNodeType() == ENodeType::ENT_Dialogue; }
	FORCEINLINE bool IsSelectNode() const { return GetNodeType() == ENodeType::ENT_Select; }
	FORCEINLINE ENodeType GetNodeType() const { return NodeType; }
	FORCEINLINE TArray<FRVNClassInfo> GetConditionNodes() { return ConditionNode; }
	FORCEINLINE TArray<FRVNClassInfo> GetTaskNodes() { return TaskNode; }

	// 插槽节点管理  
	void AddCondition(const FRVNClassInfo& InNode);
	void RemoveCondition(const FRVNClassInfo& InNode);
	void AddTask(const FRVNClassInfo& InNode);
	void RemoveTask(const FRVNClassInfo& InNode);

	// Index
	int32 GetNodeId() const;
	void SetNodeId(const int32 NewId);

public:
	// UEdGraphNode interface  
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	UEdGraphPin* GetInputPin() const;
	UEdGraphPin* GetOutputPin() const;
	// End of UEdGraphNode interface

	// Changed From Detail
	void OnPropertyEdited(const FPropertyChangedEvent& InPropertyChangedEvent) const;
	void OnSpeakerNameChanged() const;
	void OnDialogueContentChanged() const;
	void OnIsPlayerChanged() const;
	void OnPositionChanged();
	// End of Detail Interface

public:
	// 基础数据
	UPROPERTY(VisibleAnywhere, Category = "Dialogue")
	int32 NodeId;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType == ENodeType::ENT_Dialogue"), Category = "Dialogue")
	FString StateName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType != ENodeType::ENT_Entry"), Category = "Dialogue")
	FString StateContent;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType == ENodeType::ENT_Dialogue"), Category = "Dialogue")
	bool bIsPlayer;

	// 插槽节点
	UPROPERTY(VisibleAnywhere, meta = (EditCondition = "NodeType != ENodeType::ENT_Entry"), Category = "Dialogue")
	TArray<FRVNClassInfo> ConditionNode;

	UPROPERTY(VisibleAnywhere, Category = "Dialogue")
	TArray<FRVNClassInfo> TaskNode;

	UPROPERTY(VisibleAnywhere, Category = "Internal")
	ENodeType NodeType;

public:
	FOnNodeIdChanged OnNodeIdChanged;
	FOnAddCondition OnAddCondition;
	FOnAddTask OnAddTask;
	FOnRemoveCondition OnRemoveCondition;
	FOnRemoveTask OnRemoveTask;

private:
	const URVNDialogueGraph* GetDialogueGraphConst() const;

	URVNDialogueGraph* GetDialogueGraph();

private:
	UPROPERTY()
	TWeakObjectPtr<URVNDialogueGraph> RVNDialogueGraph;

	// 连接管理  
	UPROPERTY()
	TArray<URVNStateNode*> InputConnections;

	UPROPERTY()
	TArray<URVNStateNode*> OutputConnections;
};
