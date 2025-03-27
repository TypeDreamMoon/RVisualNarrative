#pragma once  

#include "CoreMinimal.h"  
#include "SGraphPin.h"

class SRVNStatePin : public SGraphPin  
{  
public:  
	SLATE_BEGIN_ARGS(SRVNStatePin) {}  
	SLATE_END_ARGS()  

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);  

protected:  
	virtual FSlateColor GetPinColor() const override;  
};  