#include "Decorator/DefaultNode/RVNAsyncTask_Delay.h"

#include "RVNComponent.h"

void URVNAsyncTask_Delay::OnStart_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask_Delay::OnStart_Implementation()!"));
}

void URVNAsyncTask_Delay::ExecuteTask_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask_Delay::ExecuteTask_Implementation()!"));

	check(OwnerComponent)

	if (const auto World = OwnerComponent->GetWorld())
	{
		FTimerHandle TimerHandle;

		World->GetTimerManager().SetTimer(TimerHandle, this, &URVNAsyncTask_Delay::FinishAsyncTask, DelayTime, false);
	}
}

void URVNAsyncTask_Delay::OnEnd_Implementation(URVNComponent* OwnerComponent)
{
	UE_LOG(LogTemp, Display, TEXT("URVNAsyncTask_Delay::OnEnd_Implementation()!"));
}
