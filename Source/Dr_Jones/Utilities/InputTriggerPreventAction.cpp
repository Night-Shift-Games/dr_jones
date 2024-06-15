// Property of Night Shift Games, all rights reserved.

#include "InputTriggerPreventAction.h"

#include "EnhancedPlayerInput.h"
#include "InputAction.h"

ETriggerState UInputTriggerPreventAction::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	const FInputActionInstance* EventData = PlayerInput->FindActionInstanceData(ActionThatPrevents);
	const ETriggerEvent EventState = EventData ? EventData->GetTriggerEvent() : ETriggerEvent::None;
	if (EventState == ETriggerEvent::Completed || EventState == ETriggerEvent::Triggered)
	{
		bTriggered = true;
	}
	else if (EventState == ETriggerEvent::Canceled || EventState == ETriggerEvent::Started)
	{
		bTriggered = false;
	}
	return bTriggered ? ETriggerState::None : ETriggerState::Triggered; 
}
