// Property of Night Shift Games, all rights reserved.

#include "DrJonesGameUserSettings.h"

#include "Sound/SoundSubmix.h"
#include "Utilities/NightShiftSettings.h"

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

void UDrJonesGameUserSettings::ApplyAudioSettings(const UObject* WorldContextObject) const
{
	const UNightShiftSettings* Settings = GetDefault<UNightShiftSettings>();
	if (!Settings)
	{
		return;
	}

	if (USoundSubmix* MasterSubmix = Settings->AudioSettings.MasterSubmix.LoadSynchronous())
	{
		MasterSubmix->SetSubmixOutputVolume(WorldContextObject, MasterVolume);
	}
	if (USoundSubmix* SFXSubmix = Settings->AudioSettings.SFXSubmix.LoadSynchronous())
	{
		SFXSubmix->SetSubmixOutputVolume(WorldContextObject, SFXVolume);
	}
	if (USoundSubmix* MusicSubmix = Settings->AudioSettings.MusicSubmix.LoadSynchronous())
	{
		MusicSubmix->SetSubmixOutputVolume(WorldContextObject, MusicVolume);
	}
}
