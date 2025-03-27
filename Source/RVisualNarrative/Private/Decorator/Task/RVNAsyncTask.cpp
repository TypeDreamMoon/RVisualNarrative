#include "Decorator/Task/RVNAsyncTask.h"

void URVNAsyncTask::OnStart_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask::OnStart_Implementation()!"));
}

void URVNAsyncTask::ExecuteTask_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask::ExecuteTask_Implementation()!"));
}

void URVNAsyncTask::OnEnd_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask::OnEnd_Implementation()!"));
}

void URVNAsyncTask::FinishAsyncTask()
{
	CompletedCallback.ExecuteIfBound();
}
