#pragma once

#include "BlueprintEditor.h"
#include "RVNAssetBlueprint.h"
#include "RVNComponent.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class URVNComponent;
class URVNAssetBlueprint;
class FDocumentTracker;
class FRVNEditorToolbar;
class URVNDialogueGraph;
class FRVNEditorToolbarBuilder;
class IDetailsView;
class SRVNNodeList;

class FRVNEditor : public FBlueprintEditor
{
public:
	FRVNEditor();

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void InitRVNEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
	                   URVNAssetBlueprint* InBlueprint);

	void RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager);

	void RestoreRVNEditor();

	void RestoreEventMode();

	void CreateDialogueGraph();

	void CreateEventGraph();

	void CreateGraphCommandList();

	void DeleteSelectedNodesDG();
	void CopySelectedNodesDG();
	void CutSelectedNodesDG();
	void PasteNodesDG();
	bool CanDeleteNodesDG() const;
	bool CanCopyNodesDG() const;
	bool CanCutNodesDG() const;
	bool CanDuplicateNodesDG() const;
	virtual void PasteNodesHere(UEdGraph* DestinationGraph, const FVector2D& GraphLocation) override;
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	void HandleUndoTransactionDG(const FTransaction* Transaction);

	void OpenDocument(UEdGraph* InGraph, FDocumentTracker::EOpenDocumentCause InOpenCause);

	virtual void OnClose() override;

	void SaveEditedObjectState();

	// 模式访问检查函数  
	bool CanAccessDialogueMode() const;
	bool CanAccessEventMode() const;

	// Delegate
	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);
	void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection);
	void OnNodeDoubleClicked(UEdGraphNode* Node);

public:
	FORCEINLINE URVNComponent* GetRVNComponent() const;

	FORCEINLINE URVNAssetBlueprint* GetRVNEditorBlueprint() const;

	FORCEINLINE URVNDialogueGraph* GetRVNDialogueGraph() const;

	FORCEINLINE TSharedPtr<FRVNEditorToolbarBuilder> GetRVNToolbarBuilder();

	FORCEINLINE TSharedPtr<FDocumentTracker> GetDocumentManager();

	TSharedRef<SWidget> SpawnDetailsView();

	TSharedRef<SWidget> SpawnNodeListView();

	TSharedRef<SGraphEditor> CreateRVNGraphEditorWidget(UEdGraph* InGraph);

protected:
	virtual void Compile() override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetBaseToolkitName() const override;

	virtual FString GetWorldCentricTabPrefix() const override;

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

private:
	void CreateInternalWidgets();

	void RefreshRVNEditor(bool bNewlyCreated);

public:
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<FUICommandList> GraphEditorCommandsRef;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SRVNNodeList> NodeListView;

private:
	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;
	TSharedPtr<FRVNEditorToolbarBuilder> RVNToolbarBuilder;

	TObjectPtr<UEdGraph> DialogueGraphPtr;
	TObjectPtr<UEdGraph> EventGraphPtr;
};
