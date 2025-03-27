#include "Graph/Node/Factory/RVNTaskFactory.h"

#include "Decorator/Task/RVNAsyncTask.h"
#include "Blueprint/RVNTaskNodeBlueprint.h"
#include "Graph/Node/Slate/SRVNNodeBlueprintCreateDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "RVNAsyncTaskFactory"

URVNTaskFactory::URVNTaskFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = URVNTaskNodeBlueprint::StaticClass();
	ParentClass = URVNTaskBase::StaticClass();
}

bool URVNTaskFactory::ConfigureProperties()
{
	TSharedRef<SRVNBlueprintCreateDialog> Dialog =
		SNew(SRVNBlueprintCreateDialog)
		.BaseClass(URVNTaskBase::StaticClass())
		.Title(LOCTEXT("CreateTaskBlueprintOptions", "Create Task Blueprint"));

	return Dialog->ConfigureProperties(this);
}

UObject* URVNTaskFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                           UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(URVNTaskNodeBlueprint::StaticClass()));

	if (BlueprintType == BPTYPE_Interface)
	{
		ParentClass = UInterface::StaticClass();
	}

	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->
		IsChildOf(URVNTaskBase::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), (ParentClass != nullptr)
			                            ? FText::FromString(ParentClass->GetName())
			                            : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::Format(LOCTEXT("CannotCreateGameplayAbilityBlueprint",
		                                           "Cannot create a Gameplay Ability Blueprint based on the class '{ClassName}'."), Args));
		return nullptr;
	}
	else
	{
		URVNTaskNodeBlueprint* NewBP = CastChecked<URVNTaskNodeBlueprint>(
			FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType,
			                                        URVNTaskNodeBlueprint::StaticClass(),
			                                        UBlueprintGeneratedClass::StaticClass(), CallingContext));

		if (NewBP)
		{
			URVNTaskNodeBlueprint* AbilityBP = URVNTaskNodeBlueprint::FindRootRVNNodeBlueprint(NewBP);
			if (AbilityBP == nullptr)
			{
				const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

				UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(
					NewBP, TEXT("Gameplay Ability Graph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
#if WITH_EDITORONLY_DATA
				if (NewBP->UbergraphPages.Num())
				{
					FBlueprintEditorUtils::RemoveGraphs(NewBP, NewBP->UbergraphPages);
				}
#endif
				FBlueprintEditorUtils::AddUbergraphPage(NewBP, NewGraph);
				NewBP->LastEditedDocuments.Add(NewGraph);
				NewGraph->bAllowDeletion = false;
			}
		}

		return NewBP;
	}
}

UObject* URVNTaskFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                           UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE
