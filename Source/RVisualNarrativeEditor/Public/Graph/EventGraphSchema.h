#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "EventGraphSchema.generated.h"

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNEventGraphSchema : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
};
