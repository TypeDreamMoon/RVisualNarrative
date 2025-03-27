#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"

class FRVNNodeEditor : public FBlueprintEditor
{
public:
	void InitNodeEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
	                    const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);

private:
	void EnsureGameplayAbilityBlueprintIsUpToDate(UBlueprint* Blueprint);

public:
	FRVNNodeEditor();

	virtual ~FRVNNodeEditor();

public:
	// IToolkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	// End of IToolkit interface

	virtual FString GetDocumentationLink() const override;

	virtual UBlueprint* GetBlueprintObj() const override;
};
