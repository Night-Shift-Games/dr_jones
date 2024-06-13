#include "NightShiftSettings.h"

#include "Audio/DefaultSoundBank.h"

UDefaultSoundBank* UNightShiftSettings::LoadDefaultSoundBank()
{
	return GetDefault<UNightShiftSettings>()->DefaultSoundBank.LoadSynchronous();
}
