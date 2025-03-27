#include "RVisualNarrative.h"

#define LOCTEXT_NAMESPACE "FRVisualNarrativeModule"

void FRVisualNarrativeModule::StartupModule()
{
}

void FRVisualNarrativeModule::ShutdownModule()
{
}

FRVisualNarrativeModule& FRVisualNarrativeModule::Get()
{
	return FModuleManager::LoadModuleChecked<FRVisualNarrativeModule>("RVisualNarrative");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRVisualNarrativeModule, RVisualNarrative)
