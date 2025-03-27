#include "Toolbar/RVNPlayToolBar.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "RVNEditorCommands.h"
#include "RVNEditorStyle.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "RVNPlayToolBar"

FRVNPlayToolBar::FRVNPlayToolBar()
	: CommandList(MakeShared<FUICommandList>())
{
	BuildAction();
}

void FRVNPlayToolBar::Initialize()
{
	const auto PlayToolBar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");

	auto& EditorSection = PlayToolBar->AddSection("RVisualNarrativeEditor");

	EditorSection.AddEntry(FToolMenuEntry::InitComboButton(
		"RVisualNarrativeEditor",
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FRVNPlayToolBar::GeneratePlayToolBarMenu),
		LOCTEXT("RVisualNarrativeEditor_Label", "RVisualNarrative"),
		LOCTEXT("RVisualNarrativeEditor_ToolTip", "RVisualNarrative"),
		FSlateIcon(FRVNEditorStyle::GetStyleSetName(), "RVNEditor.PluginAction")
	));
}

void FRVNPlayToolBar::Deinitialize()
{
}

void FRVNPlayToolBar::BuildAction()
{
	CommandList->MapAction(
		FRVNEditorCommands::Get().OpenEditorSettings,
		FExecuteAction::CreateLambda([]
		{
			if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
			{
				SettingsModule->ShowViewer("Editor", "Plugins", "RVisualNarrativeEditorSettings");
			}
		}), FCanExecuteAction());
}

TSharedRef<SWidget> FRVNPlayToolBar::GeneratePlayToolBarMenu()
{
	const FRVNEditorCommands& Commands = FRVNEditorCommands::Get();

	FMenuBuilder MenuBuilder(true, CommandList);

	MenuBuilder.BeginSection(NAME_None);

	MenuBuilder.AddMenuEntry(Commands.OpenEditorSettings, NAME_None,
	                         LOCTEXT("OpenEditorSettings", "Open EditorSettings"));

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
