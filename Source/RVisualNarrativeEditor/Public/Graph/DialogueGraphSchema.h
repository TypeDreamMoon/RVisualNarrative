#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "DialogueGraphSchema.generated.h"

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNDialogueGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	// Only consider attempting to reconnect in the same direction。
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	// The connection rules of nodes have been extensively restricted for scalability,
	// and efforts have been made to avoid the addition of new types of StateNodes that may disrupt the original rules.
	// Therefore, only A is considered as Outerput。
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
};
