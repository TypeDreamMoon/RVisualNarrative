#pragma once

#include "CoreMinimal.h"
#include "RVNDialogueManager.generated.h"

class URVNTaskBase;
class URVNComponent;
struct FRVNNodeData;

UCLASS(BlueprintType, Blueprintable)
class RVISUALNARRATIVE_API URVNDialogueManager : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueCompleted);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskCompleted);

	URVNDialogueManager();

	void Initialize(URVNComponent* InComponent);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData StartDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData StartDialogueFromId(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FRVNNodeData> GetPassedNextNodeData(TArray<int32> NextNodesId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData GetNodeDataFromId(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool CheckAllConditions(int32 InNodeId) const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ProcessAllTasks(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void BreakCurrentDialogue();

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnDialogueCompleted OnDialogueCompleted;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnTaskCompleted OnTaskCompletedCallback;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	int32 CurrentNodeId;

private:
	TWeakObjectPtr<URVNComponent> RVNComponent;

	TArray<URVNTaskBase*> CurrentTasks;
	int32 PendingProcessCount;
	int32 CompletedTaskCount;

	void OnTaskCompleted(URVNTaskBase* CompletedTask);
};
