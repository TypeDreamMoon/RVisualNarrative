#include "RVisualNarrativeCore.h"

#include "Settings/RVisualNarrativeEditorSetting.h"

#define LOCTEXT_NAMESPACE "FRVisualNarrativeCoreModule"

void FRVisualNarrativeCoreModule::StartupModule()
{
}

void FRVisualNarrativeCoreModule::ShutdownModule()
{
}

FRVisualNarrativeCoreModule& FRVisualNarrativeCoreModule::Get()  
{  
	return FModuleManager::LoadModuleChecked<FRVisualNarrativeCoreModule>("RVisualNarrativeCore");  
}  

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRVisualNarrativeCoreModule, RVisualNarrativeCore)