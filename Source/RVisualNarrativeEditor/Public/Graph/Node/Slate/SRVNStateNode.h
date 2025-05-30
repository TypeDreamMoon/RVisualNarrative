﻿#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class URVNDecorator;
class URVNTaskBase;
class URVNConditionBase;
class URVNStateNode;
class SRVNNodeIndex;
struct FRVNClassInfo;

class SMultiLineEditableTextBox;

class SRVNStateWidget : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SRVNStateWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URVNStateNode* InNode);

	void UpdateDelegate(URVNStateNode* InNode);

	TWeakObjectPtr<URVNStateNode> GetStateNode() const { return StateNode; }

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

protected:
	TSharedRef<SWidget> CreateHeaderWidget();
	TSharedRef<SWidget> CreateNodeContent();
	TSharedRef<SWidget> CreateDialogSlot();
	TSharedRef<SWidget> CreateNodeSlot(
		TSharedPtr<SVerticalBox>& OutSlotBox,
		bool& bIsExpanded,
		const FText& SlotTitle,
		FOnClicked ExpandCallback);

	FReply OnDialogSlotExpandClicked();
	FReply OnNodeSlot1ExpandClicked();
	FReply OnNodeSlot2ExpandClicked();
	void OnConditionSlotExpandedChanged(bool bIsExpanded);
	void OnTaskSlotExpandedChanged(bool bIsExpanded);
	void OnDialogTextChanged(const FText& NewText);

	void HandleNodeIdChanged();
	void HandleAddCondition(URVNConditionBase* ConditionInfo);
	void HandleRemoveCondition(URVNConditionBase* ConditionInfo);
	void HandleAddTask(URVNTaskBase* TaskInfo);
	void HandleRemoveTask(URVNTaskBase* TaskInfo);

	TSharedRef<SWidget> CreateSlotItemWidget(URVNDecorator* InItem);

private:
	TWeakObjectPtr<URVNStateNode> StateNode;

	bool bIsDialogSlotExpanded;
	bool bIsNodeSlot1Expanded;
	bool bIsNodeSlot2Expanded;

	TSharedPtr<SRVNNodeIndex> NodeIndexOverlay;
	TSharedPtr<SVerticalBox> HeaderBox;
	TSharedPtr<SBox> AvatarBox;
	TSharedPtr<STextBlock> NameText;

	TSharedPtr<SVerticalBox> DialogSlotBox;
	TSharedPtr<SVerticalBox> NodeSlot1Box;
	TSharedPtr<SVerticalBox> NodeSlot2Box;

	TMap<TObjectPtr<URVNConditionBase>, TSharedPtr<SWidget>> ConditionSlotItems;
	TMap<TObjectPtr<URVNTaskBase>, TSharedPtr<SWidget>> TaskSlotItems;

	TSharedPtr<SBox> DialogTextBox;
	TSharedPtr<SMultiLineEditableTextBox> DialogTextEditor;
	FEditableTextBoxStyle TextBoxStyle;
};
