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
	constexpr float ProbabilityFactor = 1.f / 10.f;
	const float Modifier = FMath::Pow(2.f, DaysSinceEventHasBeenTriggered * ProbabilityFactor) - 1.f;
	return DefaultProbabilityOfTrigger * Modifier;
}

UGameInstance* UEvent::GetGameInstance() const
{
	return GetTypedOuter<UEventSubsystem>()->GetGameInstance();
}

void UEvent::Trigger()
{
	LastTimeFired = GetGameInstance()->GetSubsystem<UClock>()->GetCurrentTime();
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, EventName.ToString());
}

bool UEvent::CanBeTriggeredDynamic_Implementation()
{
	return true;
}
