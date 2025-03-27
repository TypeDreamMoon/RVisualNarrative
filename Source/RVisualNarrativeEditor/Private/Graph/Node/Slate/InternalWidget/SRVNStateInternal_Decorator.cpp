#include "Graph/Node/Slate/InternalWidget/SRVNStateInternal_Decorator.h"

#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"
#include "Graph/RVNDialogueGraph.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Node/Slate/SRVNStateNode.h"

class FRVNDecoratorDragDropOp : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FRVNDecoratorDragDropOp, FDragDropOperation)

	FRVNDecoratorDragDropOp()
	{
		MouseCursor = EMouseCursor::GrabHandClosed;
	}

	void CreateDraggedWidget(const FString& InClassName)
	{
		DraggedVisualWidget = SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.3f, 0.4f))
			.Padding(FMargin(4.0f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(InClassName))
					.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.7f))
				]
			];
	}

	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override
	{
		if (DraggedVisualWidget.IsValid())
		{
			FVector2D DecoratorPosition = DragDropEvent.GetScreenSpacePosition() + FVector2D(5.0f, 5.0f);

			// If the preview window does not have a parent window yet, create one 
			if (!DraggedVisualWidget->GetParentWidget())
			{
				FSlateApplication::Get().AddWindow(
					SNew(SWindow)
					.Type(EWindowType::Notification)
					.IsPopupWindow(true)
					.SizingRule(ESizingRule::Autosized)
					.ScreenPosition(DecoratorPosition)
					[
						DraggedVisualWidget.ToSharedRef()
					]
				);
			}
			else
			{
				if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(
					DraggedVisualWidget.ToSharedRef()))
				{
					ParentWindow->MoveWindowTo(DecoratorPosition);
				}
			}
		}
	}

	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override
	{
		if (!bDropWasHandled && SourceDecorator.IsValid())
		{
			const auto DecoratorWidget = SourceDecorator.Pin();
			if (!DecoratorWidget.IsValid())
			{
				return;
			}

			const auto StateNode = DecoratorWidget->GetOuterNode().Pin();
			if (StateNode == nullptr)
			{
				return;
			}

			// Obtain geometric information and check for overlap 
			const auto StateNodeGeometry = StateNode->GetCachedGeometry();
			const auto DecoratorWidgetGeometry = DraggedVisualWidget->GetCachedGeometry();

			const auto StateSlateRect = StateNodeGeometry.GetRenderBoundingRect();
			const auto DecoratorWidgetSlateRect = DecoratorWidgetGeometry.GetRenderBoundingRect();

			// If there is no overlap, delete the decorator, and add new overlaps as well.
			if (!FSlateRect::DoRectanglesIntersect(StateSlateRect, DecoratorWidgetSlateRect))
			{
				if (SourceDecorator.IsValid() && SourceDecorator.Pin()->GetOuterNode().IsValid() && SourceDecorator.
					Pin()->GetOuterNode().Pin()->GetStateNode().IsValid())
				{
					for (auto It = SourceDecorator.Pin()->GetOuterNode().Pin()->GetStateNode()->GetGraph()->Nodes.
					                               CreateConstIterator(); It; ++It)
					{
						const auto UnderStateNode = Cast<URVNStateNode>(*It);

						const auto DialogueGraph = Cast<URVNDialogueGraph>(UnderStateNode->GetGraph());

						const auto UnderStateNodeWidget = DialogueGraph->
							GetNodeWidgetFromGuid(UnderStateNode->NodeGuid);

						const auto UnderStateNodeGeometry = UnderStateNodeWidget->GetCachedGeometry();

						if (UnderStateNodeGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
						{
							const auto OverlayDecoratorInfo = SourceDecorator.Pin()->GetDecoratorInfo();

							if (SourceDecorator.Pin()->GetDecoratorInfo().GetClass()->IsChildOf(
								URVNTaskBase::StaticClass()))
							{
								UnderStateNode->AddTask(OverlayDecoratorInfo);
							}
							else if (SourceDecorator.Pin()->GetDecoratorInfo().GetClass()->IsChildOf(
								URVNConditionBase::StaticClass()))
							{
								UnderStateNode->AddCondition(OverlayDecoratorInfo);
							}

							break;
						}
					}
				}

				DecoratorWidget->RequestDelete();
			}

			DraggedVisualWidget->SetVisibility(EVisibility::Collapsed);
		}
	}

	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
	{
		return DraggedVisualWidget;
	}

	TSharedPtr<SWidget> DraggedVisualWidget;

	TWeakPtr<SRVNStateInternal_Decorator> SourceDecorator;
};

const FLinearColor SRVNStateInternal_Decorator::ColorNormal(0.2f, 0.2f, 0.3f, 0.8f);
const FLinearColor SRVNStateInternal_Decorator::ColorHovered(0.3f, 0.3f, 0.4f, 0.8f);
const FLinearColor SRVNStateInternal_Decorator::ColorSelected(0.4f, 0.4f, 0.5f, 0.8f);
const FLinearColor SRVNStateInternal_Decorator::ColorDragging(0.3f, 0.3f, 0.4f, 0.4f);

void SRVNStateInternal_Decorator::Construct(const FArguments& InArgs)
{
	DecoratorInfo = InArgs._DecoratorInfo;
	OuterNode = InArgs._OuterNode;
	OnDeleteRequested = InArgs._OnDeleteRequested;

	ChildSlot
	[
		SAssignNew(MainBorder, SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.3f, 0.8f))
		.Padding(FMargin(4.0f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(DecoratorInfo.ClassName))
			]
		]
	];
}

void SRVNStateInternal_Decorator::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bIsSelected)
	{
		MainBorder->SetBorderBackgroundColor(ColorHovered);
	}
}

void SRVNStateInternal_Decorator::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (!bIsSelected)
	{
		MainBorder->SetBorderBackgroundColor(ColorNormal);
	}
}

FReply SRVNStateInternal_Decorator::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

FReply SRVNStateInternal_Decorator::OnDragDetected(
	const FGeometry& MyGeometry,
	const FPointerEvent& MouseEvent)
{
	TSharedRef<FRVNDecoratorDragDropOp> DragDropOp = MakeShared<FRVNDecoratorDragDropOp>();

	DragDropOp->SourceDecorator = SharedThis(this);

	DragDropOp->CreateDraggedWidget(DecoratorInfo.ClassName);

	MainBorder->SetBorderBackgroundColor(ColorDragging);

	return FReply::Handled().BeginDragDrop(DragDropOp);
}

void SRVNStateInternal_Decorator::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	MainBorder->SetBorderBackgroundColor(bIsSelected ? ColorSelected : ColorNormal);
}

FReply SRVNStateInternal_Decorator::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Delete)
	{
		RequestDelete();

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply SRVNStateInternal_Decorator::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	SetIsSelected(true);

	return FReply::Handled();
}

void SRVNStateInternal_Decorator::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	SetIsSelected(false);
}

void SRVNStateInternal_Decorator::RequestDelete() const
{
	OnDeleteRequested.ExecuteIfBound(DecoratorInfo);
}

void SRVNStateInternal_Decorator::SetIsSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (bIsSelected)
	{
		MainBorder->SetBorderBackgroundColor(ColorSelected);
	}
	else
	{
		MainBorder->SetBorderBackgroundColor(ColorNormal);
	}
}
