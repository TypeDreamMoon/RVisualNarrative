#include "Graph/Node/Slate/SRVNStatePin.h"

void SRVNStatePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)  
{  
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);  
}  

FSlateColor SRVNStatePin::GetPinColor() const  
{  
	return FSlateColor(FLinearColor::White);  
}  