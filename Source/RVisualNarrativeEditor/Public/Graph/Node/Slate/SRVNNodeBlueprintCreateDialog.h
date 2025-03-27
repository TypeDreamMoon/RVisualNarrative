#pragma once
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "SlateOptMacros.h"
#include "Graph/Node/Factory/RVNConditionFactory.h"
#include "Graph/Node/Factory/RVNTaskFactory.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "UEVersion.h"

#define LOCTEXT_NAMESPACE "SRVNNodeBlueprintCreateDialog"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SRVNBlueprintCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRVNBlueprintCreateDialog)
			: _BaseClass(nullptr)
			  , _Title(LOCTEXT("CreateNodeBlueprintOptions", "Create Node Blueprint"))
		{
		}

		SLATE_ARGUMENT(UClass*, BaseClass)
		SLATE_ARGUMENT(FText, Title)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		BaseClass = InArgs._BaseClass;
		ParentClass = BaseClass;
		DialogTitle = InArgs._Title;

		ChildSlot
		[
			SNew(SBorder)
			.Visibility(EVisibility::Visible)
			.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
				FAppStyle::Get().GetBrush
#else
				FEditorStyle::GetBrush
#endif
				("Menu.Background"))
			[
				SNew(SBox)
				.Visibility(EVisibility::Visible)
				.WidthOverride(500.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						SNew(SBorder)
						.BorderImage(
#if UE_APP_STYLE_GET_BRUSH
							FAppStyle::Get().GetBrush
#else
							FEditorStyle::GetBrush
#endif
							("ToolPanel.GroupBorder"))
						.Content()
						[
							SAssignNew(ParentClassContainer, SVerticalBox)
						]
					]

					// Ok/Cancel buttons  
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.Padding(8)
					[
						SNew(SUniformGridPanel)
						.SlotPadding(
#if UE_APP_STYLE_GET_MARGIN
							FAppStyle::Get().GetMargin
#else
							FEditorStyle::GetMargin
#endif
							("StandardDialog.SlotPadding"))
						.MinDesiredSlotWidth(
#if UE_APP_STYLE_GET_FLOAT
							FAppStyle::Get().GetFloat
#else
							FEditorStyle::GetFloat
#endif
							("StandardDialog.MinDesiredSlotWidth"))
						.MinDesiredSlotHeight(
#if UE_APP_STYLE_GET_FLOAT
							FAppStyle::Get().GetFloat
#else
							FEditorStyle::GetFloat
#endif
							("StandardDialog.MinDesiredSlotHeight"))
						+ SUniformGridPanel::Slot(0, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(
#if UE_APP_STYLE_GET_MARGIN
								FAppStyle::Get().GetMargin
#else
								FEditorStyle::GetMargin
#endif
								("StandardDialog.ContentPadding"))
							.OnClicked(this, &SRVNBlueprintCreateDialog::OkClicked)
							.Text(LOCTEXT("CreateBlueprintOk", "OK"))
						]
						+ SUniformGridPanel::Slot(1, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(
#if UE_APP_STYLE_GET_MARGIN
								FAppStyle::Get().GetMargin
#else
								FEditorStyle::GetMargin
#endif
								("StandardDialog.ContentPadding"))
							.OnClicked(this, &SRVNBlueprintCreateDialog::CancelClicked)
							.Text(LOCTEXT("CreateBlueprintCancel", "Cancel"))
						]
					]
				]
			]
		];

		MakeParentClassPicker();
	}

	/** Sets properties for the supplied Factory */
	bool ConfigureProperties(TWeakObjectPtr<UFactory> InFactory)
	{
		NodeBlueprintFactory = InFactory;

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(DialogTitle)
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false)
			.SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;

		GEditor->EditorAddModalWindow(Window);
		NodeBlueprintFactory.Reset();

		return bOkClicked;
	}

private:
	class FRVNNodeBlueprintParentFilter : public IClassViewerFilter
	{
	public:
		/** Base class that all picks must be children of */
		UClass* BaseClass;

		FRVNNodeBlueprintParentFilter() : BaseClass(nullptr)
		{
		}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass,
		                            TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return BaseClass && InClass != BaseClass && InClass->IsChildOf(BaseClass);
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
		                                    const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
		                                    TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return BaseClass && InUnloadedClassData->IsA(BaseClass) && InUnloadedClassData->IsChildOf(BaseClass);
		}
	};

	/** Creates the combo menu for the parent class */
	void MakeParentClassPicker()
	{
		FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

		// Fill in options  
		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;
		Options.bIsBlueprintBaseOnly = true;

		TSharedPtr<FRVNNodeBlueprintParentFilter> Filter = MakeShareable(new FRVNNodeBlueprintParentFilter);
		Filter->BaseClass = BaseClass;
		Options.ClassFilters.Add(Filter.ToSharedRef());

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
		                    .AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ParentClass", "Parent Class:"))
			.ShadowOffset(FVector2D(1.0f, 1.0f))
		];

		ParentClassContainer->AddSlot()
		[
			ClassViewerModule.CreateClassViewer(
				Options, FOnClassPicked::CreateSP(this, &SRVNBlueprintCreateDialog::OnClassPicked))
		];
	}

	void OnClassPicked(UClass* ChosenClass)
	{
		ParentClass = ChosenClass;
	}

	FReply OkClicked()
	{
		if (NodeBlueprintFactory.IsValid())
		{
			if (URVNTaskFactory* TaskFactory = Cast<URVNTaskFactory>(NodeBlueprintFactory.Get()))
			{
				TaskFactory->BlueprintType = BPTYPE_Normal;
				TaskFactory->ParentClass = ParentClass.Get();
			}
			else if (URVNConditionFactory* ConditionFactory = Cast<URVNConditionFactory>(NodeBlueprintFactory.Get()))
			{
				ConditionFactory->BlueprintType = BPTYPE_Normal;
				ConditionFactory->ParentClass = ParentClass.Get();
			}
		}

		CloseDialog(true);
		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UFactory> NodeBlueprintFactory;

	/** The window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The base class that all selections must derive from */
	UClass* BaseClass;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	/** Dialog title */
	FText DialogTitle;

	/** True if Ok was clicked */
	bool bOkClicked;
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
