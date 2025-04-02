#include "RVNEditor.h"
#include "EdGraphUtilities.h"
#include "RVNAssetBlueprint.h"
#include "RVNComponent.h"
#include "SBlueprintEditorToolbar.h"
#include "SGraphPanel.h"
#include "SNodePanel.h"
#include "EditorApplicationMode/RVNEditorModes.h"
#include "Graph/DialogueGraphSchema.h"
#include "Graph/EventGraphSchema.h"
#include "Graph/GraphEditorCommands.h"
#include "Graph/RVNDialogueGraph.h"
#include "Graph/Node/RVNStateNode.h"
#include "Graph/Slate/SRVNNodeList.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/DebuggerCommands.h"
#include "Tab/RVNEditorTabFactories.h"
#include "Toolbar/RVNEditorToolbar.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "UEVersion.h"

#if UE_G_EDITOR_TRANS
#include "Editor/Transactor.h"
#endif

#define LOCTEXT_NAMESPACE "RVNEditor"

FRVNEditor::FRVNEditor()
{
}

void FRVNEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FBlueprintEditor::RegisterTabSpawners(InTabManager);
}

void FRVNEditor::InitRVNEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
                               URVNAssetBlueprint* InBlueprint)
{
	check(InBlueprint)

	const bool bNewlyCreated = InBlueprint->bIsNewlyCreated;

	InBlueprint->bIsNewlyCreated = false;

	TSharedPtr<FRVNEditor> ThisPtr(SharedThis(this));

	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}

	if (!RVNToolbarBuilder.IsValid())
	{
		RVNToolbarBuilder = MakeShared<FRVNEditorToolbarBuilder>(SharedThis(this));
	}

	GetToolkitCommands()->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());

	CreateGraphCommandList();

	CreateDefaultCommands();

	RegisterMenus();

	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);

		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FRVNEditorSummoner(ThisPtr,
				FRVNEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FRVNEditor::CreateRVNGraphEditorWidget)
			));

			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	constexpr bool bCreateDefaultStandaloneMenu = true;
	constexpr bool bCreateDefaultToolbar = true;
	const auto BlueprintEditorAppName = FName(TEXT("RVNEditorApp"));

	InitAssetEditor(Mode, InitToolkitHost, BlueprintEditorAppName, FTabManager::FLayout::NullLayout,
	                bCreateDefaultStandaloneMenu,
	                bCreateDefaultToolbar, InBlueprint);


	DocumentManager->SetTabManager(TabManager.ToSharedRef());

	TArray<UBlueprint*> CastedArray{InBlueprint};

	CommonInitialization(CastedArray, false);

	InitalizeExtenders();

	RegenerateMenusAndToolbars();

	PostLayoutBlueprintEditorInitialization();

	Compile();

	FBlueprintEditorUtils::FindAndSetDebuggableBlueprintInstances();

	if (bNewlyCreated)
	{
		CreateDialogueGraph();

		CreateEventGraph();
	}

	DialogueGraphPtr = GetRVNEditorBlueprint()->DialogueGraphPtr;

	EventGraphPtr = GetRVNEditorBlueprint()->EventGraphPtr;

	Cast<URVNDialogueGraph>(DialogueGraphPtr)->RVNCompPtr = GetRVNComponent();

	Cast<URVNDialogueGraph>(DialogueGraphPtr)->RVNEditorPtr = SharedThis(this);

	if (bNewlyCreated)
	{
		DialogueGraphPtr->GetSchema()->CreateDefaultNodesForGraph(
			*GetRVNEditorBlueprint()->DialogueGraphPtr);

		EventGraphPtr->GetSchema()->CreateDefaultNodesForGraph(
			*GetRVNEditorBlueprint()->EventGraphPtr);
	}

	CreateInternalWidgets();

	AddApplicationMode(FRVNEditorApplicationModes::RVNEditorDialogueMode,
	                   MakeShared<FRVNDialogueMode>(SharedThis(this)));

	AddApplicationMode(FRVNEditorApplicationModes::RVNEditorEventMode,
	                   MakeShared<FRVNEventMode>(SharedThis(this)));

	SetCurrentMode(FRVNEditorApplicationModes::RVNEditorDialogueMode);
}

void FRVNEditor::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FRVNEditor::RestoreRVNEditor()
{
	check(GetRVNEditorBlueprint());

	if (GetRVNEditorBlueprint()->DialogueGraphPtr == nullptr)
	{
		CreateDialogueGraph();
	}

	OpenDocument(GetRVNEditorBlueprint()->DialogueGraphPtr, FDocumentTracker::RestorePreviousDocument);
}

void FRVNEditor::RestoreEventMode()
{
	check(GetRVNEditorBlueprint());

	const auto bIsNewlyCreated = GetRVNEditorBlueprint()->EventGraphPtr == nullptr;

	if (GetRVNEditorBlueprint()->EventGraphPtr == nullptr)
	{
		CreateEventGraph();
	}
}

void FRVNEditor::CreateDialogueGraph()
{
	FText DocumentNameText = LOCTEXT("DialogueGraphTabName", "DialogueGraph");

	FName DocumentName = FName(*DocumentNameText.ToString());

	GetRVNEditorBlueprint()->Modify();

	GetRVNEditorBlueprint()->DialogueGraphPtr = FBlueprintEditorUtils::CreateNewGraph(
		GetRVNEditorBlueprint(),
		DocumentName,
		URVNDialogueGraph::StaticClass(),
		URVNDialogueGraphSchema::StaticClass());
}

void FRVNEditor::CreateEventGraph()
{
	FText DocumentNameText = LOCTEXT("EventGraphTabName", "EventGraph");

	FName DocumentName = FName(*DocumentNameText.ToString());

	GetRVNEditorBlueprint()->Modify();

	GetRVNEditorBlueprint()->EventGraphPtr = FBlueprintEditorUtils::CreateNewGraph(
		GetRVNEditorBlueprint(),
		DocumentName,
		UEdGraph::StaticClass(),
		URVNEventGraphSchema::StaticClass());

	FBlueprintEditorUtils::AddUbergraphPage(GetRVNEditorBlueprint(), GetRVNEditorBlueprint()->EventGraphPtr);

	GetRVNEditorBlueprint()->EventGraphPtr->bAllowDeletion = false;
	GetRVNEditorBlueprint()->EventGraphPtr->bAllowRenaming = false;
}

void FRVNEditor::CreateGraphCommandList()
{
	FRVNGraphEditorCommands::Register();

	GraphEditorCommandsRef = MakeShareable(new FUICommandList);
	{
		GraphEditorCommandsRef->MapAction(FRVNGraphEditorCommands::Get().Delete,
		                                  FExecuteAction::CreateSP(this, &FRVNEditor::DeleteSelectedNodesDG),
		                                  FCanExecuteAction::CreateSP(this, &FRVNEditor::CanDeleteNodesDG)
		);

		GraphEditorCommandsRef->MapAction(FRVNGraphEditorCommands::Get().Copy,
		                                  FExecuteAction::CreateSP(this, &FRVNEditor::CopySelectedNodesDG),
		                                  FCanExecuteAction::CreateSP(this, &FRVNEditor::CanCopyNodesDG)
		);

		GraphEditorCommandsRef->MapAction(FRVNGraphEditorCommands::Get().Cut,
		                                  FExecuteAction::CreateSP(this, &FRVNEditor::CutSelectedNodesDG),
		                                  FCanExecuteAction::CreateSP(this, &FRVNEditor::CanCutNodesDG)
		);

		GraphEditorCommandsRef->MapAction(FRVNGraphEditorCommands::Get().Paste,
		                                  FExecuteAction::CreateSP(this, &FRVNEditor::PasteNodesDG),
		                                  FCanExecuteAction::CreateSP(this, &FRVNEditor::CanPasteNodes)
		);

		GraphEditorCommandsRef->MapAction(FRVNGraphEditorCommands::Get().Duplicate,
		                                  FExecuteAction::CreateSP(this, &FRVNEditor::DuplicateNodes),
		                                  FCanExecuteAction::CreateSP(this, &FRVNEditor::CanDuplicateNodesDG)
		);
	}
}

void FRVNEditor::DeleteSelectedNodesDG()
{
	const auto FocusedGraphEd = FocusedGraphEdPtr.Pin();
	if (!FocusedGraphEd.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	FocusedGraphEd->GetCurrentGraph()->Modify();

	const auto SelectedNodes = GraphEditor->GetSelectedNodes();

	SetUISelectionState(NAME_None);

	bool bNeedToModifyStructurally = false;

	for (const auto Node : SelectedNodes)
	{
		if (UEdGraphNode* GraphNode = Cast<UEdGraphNode>(Node))
		{
			if (const auto StateNode = Cast<URVNStateNode>(GraphNode))
			{
				GetRVNComponent()->SetFlags(RF_Transactional);

				GetRVNDialogueGraph()->RemoveStateNode(StateNode->GetNodeId());

				bNeedToModifyStructurally = true;

				AnalyticsTrackNodeEvent(GetBlueprintObj(), StateNode, true);

				GetBlueprintObj()->Modify();

				FBlueprintEditorUtils::RemoveNode(GetBlueprintObj(), StateNode, true);
			}
		}
	}

	if (bNeedToModifyStructurally)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
	}
	else
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());
	}
}

void FRVNEditor::CopySelectedNodesDG()
{
	const auto SelectedNodes = GraphEditor->GetSelectedNodes();

	TSet<UObject*> CopyStateNodes;

	FString ExportedText;

	for (const auto Node : SelectedNodes)
	{
		if (const auto StateNode = Cast<URVNStateNode>(Node))
		{
			CopyStateNodes.Add(StateNode);

			StateNode->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(CopyStateNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

void FRVNEditor::CutSelectedNodesDG()
{
	CopySelectedNodesDG();

	DeleteSelectedNodesDG();
}

void FRVNEditor::PasteNodesDG()
{
	const auto FocusedGraphEd = FocusedGraphEdPtr.Pin();
	if (!FocusedGraphEd.IsValid())
	{
		return;
	}

	PasteNodesHere(FocusedGraphEd->GetCurrentGraph(), FocusedGraphEd->GetPasteLocation());
}

bool FRVNEditor::CanDeleteNodesDG() const
{
	const auto SelectedNodes = GraphEditor->GetSelectedNodes();

	bool bCanDelete = false;

	if (IsEditable(GetFocusedGraph()) && SelectedNodes.Num() > 0)
	{
		for (const auto Node : SelectedNodes)
		{
			if (const auto StateNode = Cast<URVNStateNode>(Node))
			{
				if (StateNode->IsEntryNode())
				{
					return false;
				}

				if (StateNode->IsDialogueNode())
				{
					bCanDelete = true;
				}
				else if (StateNode->IsSelectNode())
				{
					bCanDelete = true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return bCanDelete;
}

bool FRVNEditor::CanCopyNodesDG() const
{
	const auto SelectedNodes = GraphEditor->GetSelectedNodes();

	bool bCanCopy = false;

	for (const auto Node : SelectedNodes)
	{
		if (const auto StateNode = Cast<URVNStateNode>(Node))
		{
			if (StateNode->IsEntryNode())
			{
				return false;
			}

			if (StateNode->IsDialogueNode())
			{
				bCanCopy = true;
			}
			else if (StateNode->IsSelectNode())
			{
				bCanCopy = true;
			}
			else
			{
				return false;
			}
		}
	}

	return bCanCopy;
}

bool FRVNEditor::CanCutNodesDG() const
{
	return CanDeleteNodesDG() && CanCopyNodesDG();
}

bool FRVNEditor::CanDuplicateNodesDG() const
{
	return CanCopyNodesDG() && IsEditable(GetFocusedGraph());
}

void FRVNEditor::PasteNodesHere(UEdGraph* DestinationGraph, const FVector2D& GraphLocation)
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = FocusedGraphEdPtr.Pin();
	if (!FocusedGraphEd.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
	DestinationGraph->Modify();

	SetUISelectionState(NAME_None);

	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(DestinationGraph, TextToImport, PastedNodes);

	FVector2D AvgNodePosition(0.0f, 0.0f);

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	const float InvNumNodes = 1.0f / StaticCast<float>(PastedNodes.Num());
	AvgNodePosition.X *= InvNumNodes;
	AvgNodePosition.Y *= InvNumNodes;

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		FocusedGraphEd->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + GraphLocation.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + GraphLocation.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		Node->CreateNewGuid();

		AnalyticsTrackNodeEvent(GetBlueprintObj(), Node, false);
	}

	GraphEditor->GetGraphPanel()->Update();

	TArray<URVNStateNode*> PastedStateNodes;

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		if (const auto StateNode = Cast<URVNStateNode>(*It))
		{
			PastedStateNodes.Add(StateNode);
		}
	}

	GetRVNComponent()->SetFlags(RF_Transactional);
	GetRVNDialogueGraph()->ProcessPasteNodes(PastedStateNodes);

	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());

	FocusedGraphEd->NotifyGraphChanged();
}

void FRVNEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		const FTransaction* Transaction = GEditor->Trans->GetTransaction(
			GEditor->Trans->GetQueueLength() - GEditor->Trans->GetUndoCount());

		HandleUndoTransactionDG(Transaction);
	}
}

void FRVNEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		const FTransaction* Transaction = GEditor->Trans->GetTransaction(
			GEditor->Trans->GetQueueLength() - GEditor->Trans->GetUndoCount() - 1);

		HandleUndoTransactionDG(Transaction);
	}
}

void FRVNEditor::HandleUndoTransactionDG(const FTransaction* Transaction)
{
	if (const UBlueprint* BlueprintObj = GetBlueprintObj(); BlueprintObj && Transaction)
	{
		bool bAffectsBlueprint = false;

		TArray<UObject*> TransactionObjects;
		Transaction->GetTransactionObjects(TransactionObjects);

		for (UObject* Object : TransactionObjects)
		{
			if (TransactionObjectAffectsBlueprint(Object))
			{
				bAffectsBlueprint = true;
				break;
			}
		}

		if (bAffectsBlueprint)
		{
			SetUISelectionState(NAME_None);

			RefreshEditors();

			FSlateApplication::Get().DismissAllMenus();
		}

		if (DialogueGraphPtr)
		{
			DialogueGraphPtr->NotifyGraphChanged();
		}
	}
}


void FRVNEditor::OpenDocument(UEdGraph* InGraph, FDocumentTracker::EOpenDocumentCause InOpenCause)
{
	check(InGraph != nullptr && DocumentManager.IsValid());

	const auto Payload = FTabPayload_UObject::Make(InGraph);

	DocumentManager->OpenDocument(Payload, InOpenCause);
}

void FRVNEditor::OnClose()
{
	FWorkflowCentricApplication::OnClose();
}

void FRVNEditor::SaveEditedObjectState()
{
	DocumentManager->SaveAllState();
}

bool FRVNEditor::CanAccessDialogueMode() const
{
	return true;
}

bool FRVNEditor::CanAccessEventMode() const
{
	return true;
}

void FRVNEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	FBlueprintEditor::OnGraphEditorFocused(InGraphEditor);
}

void FRVNEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	DetailsView->SetObjects(NewSelection.Array());
}

void FRVNEditor::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	if (const auto StateNode = Cast<URVNStateNode>(Node))
	{
		if (StateNode->IsEntryNode())
		{
			GetRVNComponent()->DebugPrintNodeTree();
		}
	}
}

URVNComponent* FRVNEditor::GetRVNComponent() const
{
	return Cast<URVNComponent>(GetRVNEditorBlueprint()->GeneratedClass->ClassDefaultObject);
}

URVNAssetBlueprint* FRVNEditor::GetRVNEditorBlueprint() const
{
	return Cast<URVNAssetBlueprint>(GetBlueprintObj());
}

URVNDialogueGraph* FRVNEditor::GetRVNDialogueGraph() const
{
	return Cast<URVNDialogueGraph>(DialogueGraphPtr.Get());
}

TSharedPtr<FRVNEditorToolbarBuilder> FRVNEditor::GetRVNToolbarBuilder()
{
	return RVNToolbarBuilder;
}

TSharedPtr<FDocumentTracker> FRVNEditor::GetDocumentManager()
{
	return DocumentManager;
}

TSharedRef<SWidget> FRVNEditor::SpawnDetailsView()
{
	return
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.HAlign(HAlign_Fill)
			[
				DetailsView.ToSharedRef()
			];
}

TSharedRef<SWidget> FRVNEditor::SpawnNodeListView()
{
	return NodeListView.ToSharedRef();
}

FName FRVNEditor::GetToolkitFName() const
{
	return FName("RVNEditor");
}

FText FRVNEditor::GetBaseToolkitName() const
{
	return LOCTEXT("RVNEditorToolkitName", "Visual Narrative Editor");
}

FString FRVNEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("Visual Narrative Editor");
}

FLinearColor FRVNEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.7f, 0.5f, 1.0f);
}

TSharedRef<SGraphEditor> FRVNEditor::CreateRVNGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != nullptr);

	SGraphEditor::FGraphEditorEvents InEvents;

	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(
		this, &FRVNEditor::OnSelectedNodesChanged);

	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(
		this, &FRVNEditor::OnNodeDoubleClicked);

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString("RVNDialogueGraph");

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("RVNDialogueGraphLabel", "Dialogue Graph"))
				.TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
			]
		];

	TSharedRef<SGraphEditor> LocGraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommandsRef)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);

	GraphEditor = LocGraphEditor;

	return LocGraphEditor;
}

void FRVNEditor::Compile()
{
	FBlueprintEditor::Compile();
}

void FRVNEditor::CreateInternalWidgets()
{
	//if (!DetailsView.IsValid())
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
			"PropertyEditor");
		FDetailsViewArgs DetailsViewArgs;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.NotifyHook = this;
		DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
		DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		DetailsView->SetObject(nullptr);
		DetailsView->OnFinishedChangingProperties().AddUObject(Cast<URVNDialogueGraph>(DialogueGraphPtr),
		                                                       &URVNDialogueGraph::OnPropertyChanged);
	}

	//if (!NodeListView.IsValid())
	{
		NodeListView = SNew(SRVNNodeList)
			.DialogueGraph(Cast<URVNDialogueGraph>(DialogueGraphPtr));
	}
}

void FRVNEditor::RefreshRVNEditor(bool bNewlyCreated)
{
}

#undef LOCTEXT_NAMESPACE
