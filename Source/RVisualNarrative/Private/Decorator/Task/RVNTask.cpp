#include "Decorator/Task/RVNTask.h"

#if	WITH_EDITOR
FString URVNTaskBase::GetNodeIconName() const
{
	return TEXT("BTEditor.Graph.NewTask");
}
#endif
