#pragma once

#include "CoreMinimal.h"
#include "RVNClassInfo.generated.h"

USTRUCT()
struct RVISUALNARRATIVE_API FRVNClassInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FString ClassName;

	UPROPERTY(VisibleAnywhere)
	FString ObjectPath;

	UPROPERTY(VisibleAnywhere)
	FString AssetClass;

	FRVNClassInfo() = default;

	FRVNClassInfo(const FString& InAssetName, const FString& InObjectPath = TEXT(""));

	FRVNClassInfo(const FAssetData& InAssetData);

	~FRVNClassInfo() = default;

	UClass* GetClass() const;

	bool operator==(const FRVNClassInfo& Other) const
	{
		return ObjectPath == Other.ObjectPath;
	}

	friend uint32 GetTypeHash(const FRVNClassInfo& ClassInfo)
	{
		return GetTypeHash(ClassInfo.ObjectPath);
	}
};
