// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"

#include "InputTriggerPreventAction.generated.h"

UCLASS()
class DR_JONES_API UInputTriggerPreventAction : public UInputTrigger
{
	GENERATED_BODY()

	virtual ETriggerType GetTriggerType_Implementation() const override { return ETriggerType::Implicit; }
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;
	
public:
	// The action that will prevent this action trigger's from triggering.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	TObjectPtr<const UInputAction> ActionThatPrevents = nullptr;

	bool bTriggered = false;
};
