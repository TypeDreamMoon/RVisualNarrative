#include "Settings/RVisualNarrativeEditorSetting.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "URVisualNarrativeEditorSetting"

URVisualNarrativeEditorSetting::URVisualNarrativeEditorSetting(const FObjectInitializer& ObjectInitializer)
{
}

#if WITH_EDITOR
void URVisualNarrativeEditorSetting::RegisterSettings(TArray<TSubclassOf<UObject>> InDefaultSupportedAssetClasses)
{
	if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		static TSet<FString> DefaultSupportedAssetPath =
		{
			TEXT("Game"),
			TEXT("RVisualNarrative")
		};

		static TSet<TSubclassOf<UObject>> DefaultSupportedAssetClasses;

		DefaultSupportedAssetClasses.Append(InDefaultSupportedAssetClasses);

		const auto MutableDefaultRVisualNarrativeEditorSetting = GetMutableDefault<URVisualNarrativeEditorSetting>();

		for (const auto& Path : DefaultSupportedAssetPath)
		{
			MutableDefaultRVisualNarrativeEditorSetting->SupportedAssetPath.AddUnique(Path);
		}

		for (const auto& DefaultSupportedAssetClass : DefaultSupportedAssetClasses)
		{
			MutableDefaultRVisualNarrativeEditorSetting->SupportedAssetClass.AddUnique(DefaultSupportedAssetClass);
		}

		SettingsModule->RegisterSettings("Editor",
		                                 "Plugins",
		                                 "RVisualNarrativeEditorSettings",
		                                 LOCTEXT("RVisualNarrativeEditorSettingsName",
		                                         "RVisualNarrative Editor Setting"),
		                                 LOCTEXT("RVisualNarrativeEditorSettingsDescription",
		                                         "RVisualNarrative Editor Setting"),
		                                 MutableDefaultRVisualNarrativeEditorSetting);
	}
}

void URVisualNarrativeEditorSetting::UnregisterSettings()
{
	if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor",
		                                   "Plugins",
		                                   "RVisualNarrativeEditorSettings");
	}
}
#endif

#if WITH_EDITOR
TArray<FString> URVisualNarrativeEditorSetting::GetSupportedAssetPath()
{
	return SupportedAssetPath;
}

TArray<TSubclassOf<UObject>> URVisualNarrativeEditorSetting::GetSupportedAssetClass()
{
	return SupportedAssetClass;
}
#endif

#undef LOCTEXT_NAMESPACE
