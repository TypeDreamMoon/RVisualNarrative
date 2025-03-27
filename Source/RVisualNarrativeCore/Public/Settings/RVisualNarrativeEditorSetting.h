#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RVisualNarrativeEditorSetting.generated.h"

UCLASS(config = RVisualNarrativeEditorSetting, defaultconfig, meta = (DisplayName = "RVisualNarrativeEditorSetting"))
class RVISUALNARRATIVECORE_API URVisualNarrativeEditorSetting : public UObject
{
	GENERATED_BODY()

public:
	explicit URVisualNarrativeEditorSetting(const FObjectInitializer& ObjectInitializer);

public:
#if WITH_EDITOR
	static void RegisterSettings(TArray<TSubclassOf<UObject>> InDefaultSupportedAssetClasses);

	static void UnregisterSettings();
#endif

public:
#if WITH_EDITOR
	TArray<FString> GetSupportedAssetPath();

	TArray<TSubclassOf<UObject>> GetSupportedAssetClass();
#endif

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = Asset)
	TArray<FString> SupportedAssetPath;

	UPROPERTY(Config, EditAnywhere, Category = Asset)
	TArray<TSubclassOf<UObject>> SupportedAssetClass;
#endif
};
