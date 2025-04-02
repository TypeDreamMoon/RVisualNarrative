#include "RVNDialogueManager.h"
#include "RVNComponent.h"
#include "Decorator/Task/RVNTask.h"
#include "Decorator/Task/RVNAsyncTask.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNSyncTask.h"

URVNDialogueManager::URVNDialogueManager()
	: CurrentNodeId(INDEX_NONE)
	  , PendingProcessCount(0)
	  , CompletedTaskCount(0)
{
}

void URVNDialogueManager::Initialize(URVNComponent* InComponent)
{
	RVNComponent = InComponent;

	CurrentTasks.Empty();
}

FRVNNodeData URVNDialogueManager::StartDialogue()
{
	return StartDialogueFromId(0);
}

FRVNNodeData URVNDialogueManager::StartDialogueFromId(int32 InNodeId)
{
	FRVNNodeData CurrentNode;
	if (!RVNComponent.IsValid() || !RVNComponent->GetNodeData(InNodeId, CurrentNode))
	{
		return CurrentNode;
	}

	CurrentNodeId = InNodeId;

	return CurrentNode;
}

TArray<FRVNNodeData> URVNDialogueManager::GetPassedNextNodeData(TArray<int32> NextNodesId)
{
	TArray<FRVNNodeData> Results;

	for (const auto NextNodeId : NextNodesId)
	{
		if (CheckAllConditions(NextNodeId))
		{
			Results.Add(GetNodeDataFromId(NextNodeId));
		}
	}

	return Results;
}


FRVNNodeData URVNDialogueManager::GetNodeDataFromId(int32 InNodeId)
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return NodeData;
	}

	return NodeData;
}

bool URVNDialogueManager::CheckAllConditions(int32 InNodeId) const
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return false;
	}

	for (auto CurrentCondition : NodeData.Conditions)
	{
		if (CurrentCondition == nullptr)
		{
			continue;
		}

		const auto ConditionClass = CurrentCondition->GetClass();

		if (!ConditionClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNConditionParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto CheckConditionFunc = CurrentCondition->FindFunction(TEXT("CheckCondition")))
			{
				CurrentCondition->ProcessEvent(CheckConditionFunc, &ConditionParams);
			}

			if (!ConditionParams.bIsPass)
			{
				return false;
			}
		}
		else
		{
			if (!CurrentCondition->CheckCondition_Implementation(RVNComponent.Get()))
			{
				return false;
			}
		}
	}

	return true;
}

void URVNDialogueManager::ProcessAllTasks(int32 InNodeId)
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return;
	}

	if (NodeData.Tasks.IsEmpty())
	{
		OnTaskCompletedCallback.Broadcast();
	}

	PendingProcessCount = NodeData.Tasks.Num();
	CompletedTaskCount = 0;
	CurrentTasks.Empty();
	CurrentTasks.Reserve(NodeData.Tasks.Num());

	for (auto CurrentTask : NodeData.Tasks)
	{
		if (CurrentTask == nullptr)
		{
			continue;
		}

		CurrentTasks.Add(CurrentTask);
	}

	// OnState
	for (const auto CurrentTask : CurrentTasks)
	{
		const auto TaskClass = CurrentTask->GetClass();

		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto StartTaskFunc = CurrentTask->FindFunction(TEXT("OnStart")))
			{
				CurrentTask->ProcessEvent(StartTaskFunc, &ConditionParams);
			}
		}
		else
		{
			CurrentTask->OnStart_Implementation(RVNComponent.Get());
		}
	}

	// ExecuteTask
	for (const auto CurrentTask : CurrentTasks)
	{
		if (const auto TaskClass = CurrentTask->GetClass())
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(CurrentTask))
			{
				AsyncTask->GetCompletedCallback().BindUObject(this, &URVNDialogueManager::OnTaskCompleted, CurrentTask);
			}

			if (!TaskClass->HasAnyClassFlags(CLASS_Native))
			{
				FRVNTaskParams ConditionParams;
				ConditionParams.InRVNComponent = RVNComponent.Get();

				if (const auto ExecuteTaskFunc = CurrentTask->FindFunction(TEXT("ExecuteTask")))
				{
					CurrentTask->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
				}
			}
			else
			{
				CurrentTask->ExecuteTask_Implementation(RVNComponent.Get());
			}

			if (const auto SyncTask = Cast<URVNSyncTask>(CurrentTask))
			{
				OnTaskCompleted(SyncTask);
			}
		}
	}
}

void URVNDialogueManager::BreakCurrentDialogue()
{
	for (const auto CurrentTask : CurrentTasks)
	{
		if (const auto TaskClass = CurrentTask->GetClass())
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(CurrentTask))
			{
				if (!TaskClass->HasAnyClassFlags(CLASS_Native))
				{
					FRVNTaskParams ConditionParams;
					ConditionParams.InRVNComponent = RVNComponent.Get();

					if (const auto ExecuteTaskFunc = AsyncTask->FindFunction(TEXT("BreakAsyncTask")))
					{
						AsyncTask->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
					}
				}
				else
				{
					AsyncTask->BreakAsyncTask_Implementation(RVNComponent.Get());
				}
			}
		}
	}
}

void URVNDialogueManager::OnTaskCompleted(URVNTaskBase* CompletedTask)
{
	CompletedTaskCount++;

	if (const auto AsyncTask = Cast<URVNAsyncTask>(CompletedTask))
	{
		AsyncTask->GetCompletedCallback().Unbind();
	}

	if (CompletedTaskCount >= PendingProcessCount)
	{
		for (const auto CurrentTask : CurrentTasks)
		{
			if (const auto TaskClass = CurrentTask->GetClass())
			{
				if (!TaskClass->HasAnyClassFlags(CLASS_Native))
				{
					FRVNTaskParams ConditionParams;
					ConditionParams.InRVNComponent = RVNComponent.Get();

					if (const auto EndTaskFunc = CurrentTask->FindFunction(TEXT("OnEnd")))
					{
						CurrentTask->ProcessEvent(EndTaskFunc, &ConditionParams);
					}
				}
				else
				{
					CurrentTask->OnEnd_Implementation(RVNComponent.Get());
				}
			}
		}

		OnTaskCompletedCallback.Broadcast();
	}
}
