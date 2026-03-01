#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "DesaturateCSSubsystem.generated.h"

class DesaturateSceneView;

UCLASS()
class DESATURATECOMPUTESHADER_API UDesaturateCSSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	TSharedPtr<DesaturateSceneView, ESPMode::ThreadSafe> DesaturateView;
	
};
