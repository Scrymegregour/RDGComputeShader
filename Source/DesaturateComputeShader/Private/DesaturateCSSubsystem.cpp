#include "DesaturateCSSubsystem.h"
#include "DesaturateSceneView.h"

void UDesaturateCSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	DesaturateView = FSceneViewExtensions::NewExtension<DesaturateSceneView>();
}
