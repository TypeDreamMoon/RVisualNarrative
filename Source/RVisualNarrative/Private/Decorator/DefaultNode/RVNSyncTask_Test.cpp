#include "Decorator/DefaultNode/RVNSyncTask_Test.h"

void URVNSyncTask_Test::OnStart_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask_Test::OnStart_Implementation()!"));
}

void URVNSyncTask_Test::ExecuteTask_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask_Test::ExecuteTask_Implementation()!"));
}

void URVNSyncTask_Test::OnEnd_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNSyncTask_Test::OnEnd_Implementation()!"));
}
