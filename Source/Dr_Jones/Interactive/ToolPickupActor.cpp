// Property of Night Shift Games, all rights reserved.


#include "ToolPickupActor.h"

void AToolPickupActor::Interact(APawn* Indicator)
{
	Cast<ARuntimeCharacter>(Indicator)->ToolComponent->AddItem(Tool);
	this->Destroy();
}
