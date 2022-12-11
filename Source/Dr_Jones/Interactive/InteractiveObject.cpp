// Property of Night Shift Games, all rights reserved.


#include "InteractiveObject.h"

// Add default functionality here for any IInteractiveObject functions that are not pure virtual.

void IInteractiveObject::Interact(APawn* Indicator)
{
}

FString IInteractiveObject::GetInteractSentence()
{
	return FString("Pickup");
}
