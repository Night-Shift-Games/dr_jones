// Property of Night Shift Games, all rights reserved.

#include "Event.h"

#include "World/Illuminati.h"

bool UEvent::CanBeTriggered() const
{
	return CanBeTriggeredDynamic();
}

UGameInstance* UEvent::GetGameInstance() const
{
	return GetTypedOuter<AIlluminati>()->GetGameInstance();
}

void UEvent::Trigger()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, EventName.ToString());
}

bool UEvent::CanBeTriggeredDynamic_Implementation() const
{
	return true;
}
