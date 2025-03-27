#include "Decorator/Task/RVNSyncTask.h"

void URVNSyncTask::OnStart_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask::OnStart_Implementation()!"));
}

void URVNSyncTask::ExecuteTask_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask::ExecuteTask_Implementation()!"));
}

void URVNSyncTask::OnEnd_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask::OnEnd_Implementation()!"));
}
