#include "Toolbar/RVNEditorToolbar.h"
#include "RVNEditor.h"
#include "EditorApplicationMode/RVNEditorModes.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "EditorStyleSet.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "RVNEditorToolbar"

void FRVNEditorToolbarBuilder::FillDialogueToolbar(FExtender& InExtender)
{
	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::FillModeSwitcherToolbar));
}

void FRVNEditorToolbarBuilder::FillEventToolbar(FExtender& InExtender)
{
	InExtender.AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		RVNEditorPtr.Pin()->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FRVNEditorToolbarBuilder::FillModeSwitcherToolbar));
}

void FRVNEditorToolbarBuilder::FillModeSwitcherToolbar(FToolBarBuilder& InToolbarBuilder)
{
	TAttribute<FName> GetActiveMode(RVNEditorPtr.Pin().Get(), &FRVNEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(
		RVNEditorPtr.Pin().Get(), &FRVNEditor::SetCurrentMode);

	InToolbarBuilder.AddWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	InToolbarBuilder.AddWidget(
		SNew(SModeWidget,
		     FRVNEditorApplicationModes::GetLocalizedMode(FRVNEditorApplicationModes::RVNEditorDialogueMode),
		     FRVNEditorApplicationModes::RVNEditorDialogueMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(RVNEditorPtr.Pin().Get(), &FRVNEditor::CanAccessDialogueMode)
		.ToolTipText(LOCTEXT("DialogueModeButtonTooltip", "Switch to conversation mode"))
		.IconImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("BTEditor.SwitchToBehaviorTreeMode"))
	);

	InToolbarBuilder.AddWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));

	InToolbarBuilder.AddWidget(
		SNew(SModeWidget, FRVNEditorApplicationModes::GetLocalizedMode(FRVNEditorApplicationModes::RVNEditorEventMode),
		     FRVNEditorApplicationModes::RVNEditorEventMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(RVNEditorPtr.Pin().Get(), &FRVNEditor::CanAccessEventMode)
		.ToolTipText(LOCTEXT("EventModeButtonTooltip", "Switch to event mode"))
		.IconImage(
#if UE_APP_STYLE_GET_BRUSH
			FAppStyle::Get().GetBrush
#else
			FEditorStyle::GetBrush
#endif
			("BTEditor.SwitchToBlackboardMode"))
	);

	InToolbarBuilder.AddWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
}

#undef LOCTEXT_NAMESPACE
