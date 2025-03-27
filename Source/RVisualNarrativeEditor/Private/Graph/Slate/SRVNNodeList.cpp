#include "Graph/Slate/SRVNNodeList.h"

#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"
#include "RVisualNarrativeEditor.h"
#include "ClassCollector/RVNClassCollector.h"
#include "Graph/RVNDialogueGraph.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/Slate/SRVNStateNode.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SRVNDialogueNodeList"

TSharedRef<FRVNNodeDragDropOp> FRVNNodeDragDropOp::New(TSharedPtr<FRVNClassInfo> InNode, URVNDialogueGraph* InGraph)
{
	TSharedRef<FRVNNodeDragDropOp> Operation = MakeShared<FRVNNodeDragDropOp>();
	Operation->DraggedNode = InNode;
	Operation->DialogueGraph = InGraph;

	Operation->DecoratorWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
		.BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.3f, 0.7f))
		.Padding(FMargin(4.0f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 4, 0)
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush(
					*Cast<URVNDecorator>(InNode->GetClass()->ClassDefaultObject)->GetNodeIconName()))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(6, 0, 4, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InNode->ClassName))
			]
		];

	Operation->Construct();
	return Operation;
}

void FRVNNodeDragDropOp::OnDragged(const FDragDropEvent& DragDropEvent)
{
	FVector2D ScreenPosition = DragDropEvent.GetScreenSpacePosition();
	if (DecoratorWidget.IsValid())
	{
		FVector2D DecoratorPosition = ScreenPosition + FVector2D(5.0f, 5.0f);

		if (!DecoratorWidget->GetParentWidget())
		{
			FSlateApplication::Get().AddWindow(
				SNew(SWindow)
				.Type(EWindowType::Notification)
				.IsPopupWindow(true)
				.SizingRule(ESizingRule::Autosized)
				.ScreenPosition(DecoratorPosition)
				[
					DecoratorWidget.ToSharedRef()
				]
			);
		}
		else
		{
			if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(
				DecoratorWidget.ToSharedRef()))
			{
				ParentWindow->MoveWindowTo(DecoratorPosition);
			}
		}

		check(DraggedNode.IsValid() && DialogueGraph.IsValid())

		DecoratorWidget->SetVisibility(EVisibility::Visible);
	}
}

void FRVNNodeDragDropOp::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (DecoratorWidget.IsValid())
	{
		DecoratorWidget->SetVisibility(EVisibility::Collapsed);
	}

	for (auto It = DialogueGraph->Nodes.CreateConstIterator(); It; ++It)
	{
		const auto StateNode = Cast<URVNStateNode>(*It);

		const auto StateNodeGeometry = StateNode->GetStateWidget()->GetCachedGeometry();

		if (StateNodeGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			OverlappingNode = StateNode;
		}
	}

	if (OverlappingNode.IsValid())
	{
		if (DraggedNode->GetClass()->IsChildOf(URVNTaskBase::StaticClass()))
		{
			OverlappingNode->AddTask(*DraggedNode);
		}
		else if (DraggedNode->GetClass()->IsChildOf(URVNConditionBase::StaticClass()))
		{
			OverlappingNode->AddCondition(*DraggedNode);
		}
	}

	FGraphEditorDragDropAction::OnDrop(bDropWasHandled, MouseEvent);
}

FCursorReply FRVNNodeDragDropOp::OnCursorQuery()
{
	return FCursorReply::Cursor(EMouseCursor::GrabHand);
}


void SRVNNodeList::Construct(const FArguments& InArgs)
{
	DialogueGraph = InArgs._DialogueGraph;

	CollectNodes();

	constexpr FLinearColor DarkBackground(0.08f, 0.08f, 0.08f, 0.2f);

	ChildSlot
	[
		SNew(SBorder)
		             .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
		             .BorderBackgroundColor(DarkBackground) 
		             .Padding(FMargin(2))
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(4)
			[
				SAssignNew(SearchBox, SSearchBox)
				.OnTextChanged(this, &SRVNNodeList::OnSearchTextChanged)
				.HintText(LOCTEXT("SearchNodes", "Search nodes..."))
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SScrollBox)
				.Style(&FAppStyle::Get().GetWidgetStyle<FScrollBoxStyle>("DarkScrollBox")) // 使用深色滚动条样式  
				+ SScrollBox::Slot()
				[
					SAssignNew(CategoryList, SVerticalBox)
				]
			]
		]
	];

	RefreshList();
}

void SRVNNodeList::OnSearchTextChanged(const FText& NewText)
{
}

void SRVNNodeList::CollectNodes()
{
	check(DialogueGraph.IsValid())

	NodeCategories.Empty();

	NodeCategories.Add(TEXT("Task"), MakeShared<FRVNNodeCategory>(TEXT("Task")));
	NodeCategories.Add(TEXT("Condition"), MakeShared<FRVNNodeCategory>(TEXT("Condition")));

	TArray<UClass*> TaskClasses;
	FRVisualNarrativeEditorModule::Get().GetClasses(URVNTaskBase::StaticClass(), TaskClasses);

	for (UClass* Class : TaskClasses)
	{
		if (!Class->HasAnyClassFlags(CLASS_Abstract))
		{
			TSharedPtr<FRVNClassInfo> NodeInfo = MakeShared<FRVNClassInfo>(
				Class->GetDisplayNameText().ToString(),
				Class->GetPathName()
			);
			NodeCategories[TEXT("Task")]->Nodes.Add(NodeInfo);
		}
	}

	TArray<UClass*> ConditionClasses;
	FRVisualNarrativeEditorModule::Get().GetClasses(URVNConditionBase::StaticClass(), ConditionClasses);
	for (UClass* Class : ConditionClasses)
	{
		if (!Class->HasAnyClassFlags(CLASS_Abstract))
		{
			TSharedPtr<FRVNClassInfo> NodeInfo = MakeShared<FRVNClassInfo>(
				Class->GetDisplayNameText().ToString(),
				Class->GetPathName()
			);
			NodeCategories[TEXT("Condition")]->Nodes.Add(NodeInfo);
		}
	}

	RefreshList();
}

void SRVNNodeList::RefreshList()
{
	if (!CategoryList.IsValid())
		return;

	CategoryList->ClearChildren();

	const FString SearchText = SearchBox.IsValid() ? SearchBox->GetText().ToString().ToLower() : FString();

	for (auto& CategoryPair : NodeCategories)
	{
		TSharedPtr<FRVNNodeCategory>& Category = CategoryPair.Value;

		if (!SearchText.IsEmpty())
		{
			bool bHasMatchingNode = false;
			for (const auto& NodeInfo : Category->Nodes)
			{
				if (NodeInfo->ClassName.Contains(SearchText))
				{
					bHasMatchingNode = true;
					break;
				}
			}
			if (!bHasMatchingNode)
				continue;
		}

		CategoryList->AddSlot()
		            .AutoHeight()
		            .Padding(0, 2)
		[
			CreateCategoryWidget(Category)
		];
	}
}

TSharedRef<SWidget> SRVNNodeList::CreateCategoryWidget(TSharedPtr<FRVNNodeCategory> Category)
{
	return SNew(SExpandableArea)
		.BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.3f))
		.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.HeaderContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(Category->CategoryName))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
		]
		.BodyContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateNodesForCategory(Category)
			]
		];
}

TSharedRef<SWidget> SRVNNodeList::CreateNodesForCategory(TSharedPtr<FRVNNodeCategory> Category)
{
	TSharedRef<SVerticalBox> NodesBox = SNew(SVerticalBox);

	const FString SearchText = SearchBox.IsValid() ? SearchBox->GetText().ToString() : FString();

	for (const auto& NodeInfo : Category->Nodes)
	{
		if (!SearchText.IsEmpty() && !NodeInfo->ClassName.Contains(SearchText))
			continue;

		NodesBox->AddSlot()
		        .AutoHeight()
		        .Padding(2, 2)
		[
			CreateNodeWidget(NodeInfo)
		];
	}

	return NodesBox;
}

TSharedRef<SWidget> SRVNNodeList::CreateNodeWidget(TSharedPtr<FRVNClassInfo> NodeInfo)
{
	TSharedRef<SWidget> NewNodeWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
		.BorderBackgroundColor_Lambda([NodeInfo, this]() -> FSlateColor
		{
			return HoveredNode == NodeInfo
				       ? FLinearColor(0.4f, 0.4f, 1.0f, 0.4f)
				       : FLinearColor(0.2f, 0.2f, 0.2f, 0.2f);
		})
		.ToolTipText(FText::FromString(NodeInfo->ClassName))
		.Padding(FMargin(4.0f))
		.OnMouseMove(FPointerEventHandler::CreateLambda(
			[this, NodeInfo](const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply
			{
				if (HoveredNode != NodeInfo)
				{
					HoveredNode = NodeInfo;
					return FReply::Handled();
				}
				return FReply::Unhandled();
			}))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 4, 0)
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush(
					*Cast<URVNDecorator>(NodeInfo->GetClass()->ClassDefaultObject)->GetNodeIconName()))
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(10, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(NodeInfo->ClassName))
				.ColorAndOpacity(FLinearColor::White)
			]
		];

	NodeWidgets.Add(NodeInfo, NewNodeWidget);

	return NewNodeWidget;
}

FReply SRVNNodeList::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		TSharedPtr<FRVNClassInfo> NodeUnderMouse = GetNodeUnderMouse(MyGeometry, MouseEvent);
		if (NodeUnderMouse.IsValid())
		{
			return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
		}
	}
	return FReply::Unhandled();
}

FReply SRVNNodeList::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<FRVNClassInfo> NodeToDrag = GetNodeUnderMouse(MyGeometry, MouseEvent);
	if (NodeToDrag.IsValid())
	{
		return FReply::Handled().BeginDragDrop(FRVNNodeDragDropOp::New(NodeToDrag, DialogueGraph.Get()));
	}
	return FReply::Unhandled();
}

TSharedPtr<FRVNClassInfo> SRVNNodeList::GetNodeUnderMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	for (const auto& NodeWidget : NodeWidgets)
	{
		if (NodeWidget.Key.IsValid())
		{
			const FGeometry& NodeGeometry = NodeWidget.Value->GetCachedGeometry();

			const auto NodeRect = NodeGeometry.GetRenderBoundingRect();

			const auto bIsOverlay = NodeRect.ContainsPoint(MouseEvent.GetScreenSpacePosition());

			if (bIsOverlay)
			{
				return NodeWidget.Key;
			}
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
