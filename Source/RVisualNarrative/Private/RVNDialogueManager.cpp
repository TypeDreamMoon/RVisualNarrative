#include "RVNDialogueManager.h"

#include "RVNComponent.h"
#include "Decorator/Task/RVNTask.h"
#include "Decorator/Task/RVNAsyncTask.h"
#include "RVNClassInfo.h"
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

	for (auto ConditionInfo : NodeData.Conditions)
	{
		const auto ConditionClass = ConditionInfo.GetClass();

		if (ConditionClass == nullptr)
		{
			continue;
		}

		const auto ConditionCDO = Cast<URVNConditionBase>(ConditionClass->ClassDefaultObject);

		if (ConditionCDO == nullptr)
		{
			continue;
		}

		if (!ConditionClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNConditionParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto CheckConditionFunc = ConditionCDO->FindFunction(TEXT("CheckCondition")))
			{
				ConditionCDO->ProcessEvent(CheckConditionFunc, &ConditionParams);
			}

			if (!ConditionParams.bIsPass)
			{
				return false;
			}
		}
		else
		{
			if (!ConditionCDO->CheckCondition_Implementation(RVNComponent.Get()))
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

	for (auto TaskInfo : NodeData.Tasks)
	{
		const auto TaskClass = TaskInfo.GetClass();

		if (TaskClass == nullptr)
		{
			continue;
		}

		CurrentTasks.Add(TaskClass);
	}

	// OnState
	for (auto TaskClass : CurrentTasks)
	{
		const auto TaskCDO = Cast<URVNTaskBase>(TaskClass->ClassDefaultObject);

		if (TaskCDO == nullptr)
		{
			continue;
		}

		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto StartTaskFunc = TaskCDO->FindFunction(TEXT("OnStart")))
			{
				TaskCDO->ProcessEvent(StartTaskFunc, &ConditionParams);
			}
		}
		else
		{
			TaskCDO->OnStart_Implementation(RVNComponent.Get());
		}
	}

	// ExecuteTask
	for (auto TaskClass : CurrentTasks)
	{
		if (const auto TaskCDO = Cast<URVNTaskBase>(TaskClass->ClassDefaultObject))
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(TaskCDO))
			{
				AsyncTask->GetCompletedCallback().BindUObject(this, &URVNDialogueManager::OnTaskCompleted, TaskCDO);
			}

			if (!TaskClass->HasAnyClassFlags(CLASS_Native))
			{
				FRVNTaskParams ConditionParams;
				ConditionParams.InRVNComponent = RVNComponent.Get();

				if (const auto ExecuteTaskFunc = TaskCDO->FindFunction(TEXT("ExecuteTask")))
				{
					TaskCDO->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
				}
			}
			else
			{
				TaskCDO->ExecuteTask_Implementation(RVNComponent.Get());
			}

			if (const auto SyncTask = Cast<URVNSyncTask>(TaskCDO))
			{
				OnTaskCompleted(TaskCDO);
			}
		}
	}
}

void URVNDialogueManager::BreakCurrentDialogue()
{
	for (auto TaskClass : CurrentTasks)
	{
		if (const auto TaskCDO = Cast<URVNTaskBase>(TaskClass->ClassDefaultObject))
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(TaskCDO))
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
		for (auto TaskClass : CurrentTasks)
		{
			if (const auto TaskCDO = Cast<URVNTaskBase>(TaskClass->ClassDefaultObject))
			{
				if (!TaskClass->HasAnyClassFlags(CLASS_Native))
				{
					FRVNTaskParams ConditionParams;
					ConditionParams.InRVNComponent = RVNComponent.Get();

					if (const auto EndTaskFunc = TaskCDO->FindFunction(TEXT("OnEnd")))
					{
						TaskCDO->ProcessEvent(EndTaskFunc, &ConditionParams);
					}
				}
				else
				{
					TaskCDO->OnEnd_Implementation(RVNComponent.Get());
				}
			}
		}

		OnTaskCompletedCallback.Broadcast();
	}
}
