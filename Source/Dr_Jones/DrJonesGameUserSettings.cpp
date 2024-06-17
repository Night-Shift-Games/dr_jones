// Property of Night Shift Games, all rights reserved.

#include "DrJonesGameUserSettings.h"

UDrJonesGameUserSettings* UDrJonesGameUserSettings::GetDrJonesGameUserSettings()
{
	return Cast<UDrJonesGameUserSettings>(GEngine->GetGameUserSettings());
}

TArray<FIntPoint> UDrJonesGameUserSettings::GetSupportedDisplayResolutions()
{
	TSet<FIntPoint> Resolutions;
	FScreenResolutionArray RHIResolutions;
	RHIGetAvailableResolutions(RHIResolutions, false);
	for (const FScreenResolutionRHI& ScreenResolutionRHI : RHIResolutions)
	{
		Resolutions.Add(FIntPoint(ScreenResolutionRHI.Width, ScreenResolutionRHI.Height));
	}

	return Resolutions.Array();
}
