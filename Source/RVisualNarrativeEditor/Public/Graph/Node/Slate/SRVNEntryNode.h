#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SRVNEntryWidget : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SRVNEntryWidget)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};