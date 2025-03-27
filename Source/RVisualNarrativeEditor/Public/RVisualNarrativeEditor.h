#pragma once

#include "Modules/ModuleInterface.h"
#include "AssetTypeCategories.h"

class FRVNClassCollector;

class FRVisualNarrativeEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	FORCEINLINE static EAssetTypeCategories::Type GetRVNAssetCategoryType() { return RVNAssetCategoryType; }

public:
	static FRVisualNarrativeEditorModule& Get();

	void GetClasses(UClass* KeyClass, TArray<UClass*>& OutClasses);

private:
	void OnPostEngineInit();
	
private:
	void RegisterRVNAsset();

	void RegisterMenus();

private:
	TSharedPtr<FRVNClassCollector> ClassCollector;

private:  
	static EAssetTypeCategories::Type RVNAssetCategoryType;

	TSharedPtr<class FRVNPlayToolBar> RVNPlayToolBar;

	TSharedPtr<class FRVNBlueprintToolBar> RVNBlueprintToolBar;

	FDelegateHandle OnPostEngineInitDelegateHandle;
};
