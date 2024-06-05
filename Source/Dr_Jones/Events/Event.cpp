// Property of Night Shift Games, all rights reserved.

#include "Event.h"

#include "EventSubsystem.h"
#include "World/Illuminati.h"

bool UEvent::CanBeTriggered()
{
	return CanBeTriggeredDynamic();
}

float UEvent::GetTriggerProbability() const
{
	const float DaysSinceEventHasBeenTriggered = (GetGameInstance()->GetSubsystem<UClock>()->GetCurrentTime() - LastTimeFired).GetDays();
	const float ProbabilityFactor = 1.f / EventRegenerationFactor;
	const float Modifier = FMath::Pow(2.f, DaysSinceEventHasBeenTriggered * ProbabilityFactor) - 1.f;
	return DefaultProbabilityOfTrigger * Modifier;
}

UGameInstance* UEvent::GetGameInstance() const
{
	return GetTypedOuter<UEventSubsystem>()->GetGameInstance();
}

UWorld* UEvent::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && ensureMsgf(GetOuter(), TEXT("Event: %s has a null OuterPrivate in UEvent::GetWorld()"), *GetFullName())
		&& !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UEvent::Trigger()
{
	LastTimeFired = GetGameInstance()->GetSubsystem<UClock>()->GetCurrentTime();
	TriggerDynamic();
}

bool UEvent::CanBeTriggeredDynamic_Implementation()
{
	return true;
}
