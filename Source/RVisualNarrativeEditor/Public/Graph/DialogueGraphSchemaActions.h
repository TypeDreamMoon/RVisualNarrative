#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "DialogueGraphSchemaActions.generated.h"

USTRUCT()
struct RVISUALNARRATIVEEDITOR_API FDGSchemaStateNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FDGSchemaStateNodeAction()
	{
	};

	FDGSchemaStateNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	                                    bool bSelectNewNode = true) override;
};

USTRUCT()
struct RVISUALNARRATIVEEDITOR_API FDGSchemaSelectorNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FDGSchemaSelectorNodeAction()
	{
	};

	FDGSchemaSelectorNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	                                    bool bSelectNewNode = true) override;
};
