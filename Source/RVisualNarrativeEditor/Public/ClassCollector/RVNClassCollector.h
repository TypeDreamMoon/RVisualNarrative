#pragma once

#include "CoreMinimal.h"
#include "RVNClassInfo.h"

class RVISUALNARRATIVEEDITOR_API FRVNClassCollector : public TSharedFromThis<FRVNClassCollector>
{
public:
	FRVNClassCollector();

	virtual ~FRVNClassCollector();

	static void GetClasses(const UClass* KeyClass, TArray<UClass*>& OutClasses);

private:
	static void PopulateClasses();

	static void PopulateClassInMemory();

	static void PopulateClassByAsset();

	static void PopulateClassInMemoryByClass(UClass* BaseClass);

	static void AssetAdded(const FAssetData& InAssetData);

	static void RemoveAsset(const FAssetData& InAssetData);

	static void OnAssetRenamed(const FAssetData& InAssetData, const FString& InOldObjectPath);

	static void ProcessSupportedAsset(
		const TFunction<void(const TArray<UClass*>&, const TArray<FName>&)>& InActuallyCheckAssetClass);

	static bool IsSupportedAssetData(const TArray<FName>& InSupportedAssetClassNames, const FAssetData& InAssetData);

private:
	static TMultiMap<UClass*, FRVNClassInfo> Classes;

	static FDelegateHandle OnAssetAddedDelegateHandle;

	static FDelegateHandle OnAssetRemovedDelegateHandle;

	static FDelegateHandle OnAssetRenamedDelegateHandle;
};
