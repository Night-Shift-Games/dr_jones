// Property of Night Shift Games, all rights reserved.

#include "DrJonesGameUserSettings.h"

UDrJonesGameUserSettings* UDrJonesGameUserSettings::GetDrJonesGameUserSettings()
{
	return Cast<UDrJonesGameUserSettings>(GEngine->GetGameUserSettings());
}
