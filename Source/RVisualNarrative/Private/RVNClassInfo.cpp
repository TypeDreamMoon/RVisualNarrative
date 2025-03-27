#include "RVNClassInfo.h"
#include "UEVersion.h"

FRVNClassInfo::FRVNClassInfo(const FString& InAssetName, const FString& InObjectPath)
	: ClassName(InAssetName)
	  , ObjectPath(InObjectPath)
{
}

FRVNClassInfo::FRVNClassInfo(const FAssetData& InAssetData)
{
	InAssetData.GetTagValue(TEXT("GeneratedClass"), ClassName);

	int32 DotIndex;

	ClassName.FindLastChar('.', DotIndex);

	if (DotIndex != INDEX_NONE)
	{
		ClassName = ClassName.Mid(DotIndex + 1).TrimEnd();

		ClassName.RemoveFromEnd(TEXT("'"));
	}

	ObjectPath = InAssetData.
#if UE_ASSET_DATA_GET_SOFT_OBJECT_PATH_STRING
		GetSoftObjectPath()
#else
		ObjectPath
#endif
		.ToString();

	AssetClass = InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
	             AssetClassPath.GetAssetName()
#else
				 AssetClass
#endif
		.ToString();
}

UClass* FRVNClassInfo::GetClass() const
{
	if (ClassName.Equals(TEXT("None")))
	{
		return nullptr;
	}

	if (const auto FindClass = Cast<UClass>(StaticFindObject(UClass::StaticClass(), nullptr, *ObjectPath)))
	{
		return FindClass;
	}

	UClass* LoadedClass = nullptr;

	{
		UObject* LoadedObject = LoadObject<UObject>(nullptr, *ObjectPath);
		if (LoadedObject == nullptr)
		{
			const FString NewPath = FLinkerLoad::FindNewNameForClass(*AssetClass, false).ToString();

			LoadedObject = LoadObject<UObject>(nullptr, *NewPath);
		}

		if (LoadedObject == nullptr)
		{
			const FString NewPath = ObjectPath + TEXT("_C");

			LoadedObject = LoadObject<UObject>(nullptr, *NewPath);
		}

		if (const auto Blueprint = Cast<UBlueprint>(LoadedObject))
		{
			LoadedClass = Blueprint->GeneratedClass;
		}
	}

	{
		if (LoadedClass == nullptr)
		{
			LoadedClass = LoadObject<UClass>(nullptr, *ObjectPath);
		}
	}

	return LoadedClass;
}
