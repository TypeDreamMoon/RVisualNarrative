#include "Tab/RVNEditorTabFactories.h"
#include "Tab/RVNEditorTabsID.h"
#include "RVNEditor.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "RVNEditorTabFactories"

FRVNEditorSummoner::FRVNEditorSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr,
                                       FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	: FDocumentTabFactoryForObjects(FRVNEditorTabsID::GraphEditor, InRVNEditorPtr),
	  RVNEditorPtr(InRVNEditorPtr),
	  OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{
}

void FRVNEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	if (!RVNEditorPtr.IsValid())
	{
		return;
	}

	const auto GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	RVNEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FRVNEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	const auto GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FRVNEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FRVNEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info,
                                                               UEdGraph* DocumentID) const
{
	if (!OnCreateGraphEditorWidget.IsBound())
	{
		return SNullWidget::NullWidget;
	}

	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FRVNEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info,
                                                           UEdGraph* DocumentID) const
{
	return FAppStyle::Get().GetBrush("BTEditor.BehaviorTree");
}

FDialogueNodeListSummoner::FDialogueNodeListSummoner(TSharedPtr<FRVNEditor> InRVNEditor)
	: FWorkflowTabFactory(FRVNEditorTabsID::DialogueNewNodeList, InRVNEditor)
	  , RVNEditorPtr(InRVNEditor)
{
	TabLabel = LOCTEXT("DialogueNodeListLabel", "Node List");

	TabIcon = FSlateIcon(
#if UE_APP_STYLE_GET_STYLE_SET_NAME
		FAppStyle::Get().GetStyleSetName()
#else
		FEditorStyle::GetStyleSetName()
#endif
		, "Kismet.Tabs.Palette");
}

TSharedRef<SWidget> FDialogueNodeListSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!RVNEditorPtr.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	return RVNEditorPtr.Pin().Get()->SpawnNodeListView();
}

FText FDialogueNodeListSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("DialogueNodeListTabToolTip", "List of available dialogue nodes");
}

FRVNDialogueDetailsSummoner::FRVNDialogueDetailsSummoner(TSharedPtr<FRVNEditor> InRVNEditorPtr)
	: FWorkflowTabFactory(FRVNEditorTabsID::DialogueNodeDetails, InRVNEditorPtr),
	  RVNEditorPtr(InRVNEditorPtr)
{
	TabLabel = LOCTEXT("DialogueNodeDetailsLabel", "Details");

	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");

	bIsSingleton = true;
}

TSharedRef<SWidget> FRVNDialogueDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!RVNEditorPtr.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	return RVNEditorPtr.Pin().Get()->SpawnDetailsView();
}

FText FRVNDialogueDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("RVNDialogueNodeDetailsTabTooltip",
	               "The RVN details tab allows editing of the properties of dialogue nodes");
}

#undef LOCTEXT_NAMESPACE
