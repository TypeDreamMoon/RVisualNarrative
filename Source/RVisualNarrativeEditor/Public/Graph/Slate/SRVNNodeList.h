#pragma once

#include "CoreMinimal.h"
#include "GraphEditorDragDropAction.h"
#include "Widgets/SCompoundWidget.h"

class URVNStateNode;
class URVNDialogueGraph;

struct FRVNClassInfo;

struct FRVNNodeCategory
{
	FString CategoryName;
	TArray<TSharedPtr<FRVNClassInfo>> Nodes;
	bool bIsExpanded;

	FRVNNodeCategory(const FString& InName)
		: CategoryName(InName), bIsExpanded(true)
	{
	}
};

class FRVNNodeDragDropOp : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FRVNNodeDragDropOp, FGraphEditorDragDropAction)

	TSharedPtr<FRVNClassInfo> DraggedNode;

	TSharedPtr<SBorder> DecoratorWidget;

	TWeakObjectPtr<URVNStateNode> OverlappingNode;

	TWeakObjectPtr<URVNDialogueGraph> DialogueGraph;

	static TSharedRef<FRVNNodeDragDropOp> New(TSharedPtr<FRVNClassInfo> InNode, URVNDialogueGraph* InGraph);

	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;

	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;

	virtual FCursorReply OnCursorQuery() override;
};

class SRVNNodeList : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SRVNNodeList)
		{
		}

		SLATE_ARGUMENT(URVNDialogueGraph*, DialogueGraph)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<URVNDialogueGraph> DialogueGraph;

	TMap<FString, TSharedPtr<FRVNNodeCategory>> NodeCategories;
	TArray<TSharedPtr<FRVNNodeCategory>> FilteredCategories;
	TSharedPtr<FRVNClassInfo> HoveredNode;
	TMap<TSharedPtr<FRVNClassInfo>, TSharedRef<SWidget>> NodeWidgets;

	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SVerticalBox> CategoryList;

	void OnSearchTextChanged(const FText& NewText);

	void CollectNodes();

	void RefreshList();
	TSharedRef<SWidget> CreateCategoryWidget(TSharedPtr<FRVNNodeCategory> Category);
	TSharedRef<SWidget> CreateNodesForCategory(TSharedPtr<FRVNNodeCategory> Category);
	TSharedRef<SWidget> CreateNodeWidget(TSharedPtr<FRVNClassInfo> NodeInfo);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	TSharedPtr<FRVNClassInfo> GetNodeUnderMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
};
