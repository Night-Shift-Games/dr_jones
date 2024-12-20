﻿// Property of Night Shift Games, all rights reserved.

#include "EventSubsystem.h"

#include "Utilities/NightShiftSettings.h"
#include "World/Clock.h"

void UEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<TSoftClassPtr<UEvent>> EventsToCreate = GetDefault<UNightShiftSettings>()->Events;
	TArray<TSoftClassPtr<UEvent>> StaticEventsToCreate = GetDefault<UNightShiftSettings>()->StaticEvents;

	for (TSoftClassPtr<UEvent> EventSoftClass : EventsToCreate)
	{
		TSubclassOf<UEvent> EventClass = EventSoftClass.LoadSynchronous();
		UEvent* NewEvent = NewObject<UEvent>(this, EventClass);
		Events.Emplace(EventClass, NewEvent);
	}

	for (TSoftClassPtr<UEvent> EventSoftClass : StaticEventsToCreate)
	{
		TSubclassOf<UEvent> EventClass = EventSoftClass.LoadSynchronous();
		UEvent* NewEvent = NewObject<UEvent>(this, EventClass);
		StaticEvents.Emplace(EventClass, NewEvent);
	}

	GetGameInstance()->GetSubsystem<UClock>()->OnClockNativeTick.AddWeakLambda(this, [&](const FDateTime CurrentTime)
	{
		if (CurrentTime.GetTimeOfDay().GetMinutes() % 15 == 0)
		{
			HandleEvents(CurrentTime);
		}
	});
}

void UEventSubsystem::Deinitialize()
{
	Events.Empty();

	GetGameInstance()->GetSubsystem<UClock>()->OnClockNativeTick.RemoveAll(this);
	
	Super::Deinitialize();
}

UEvent* UEventSubsystem::SampleRandomEvents(const float TriggerChanceModifier)
{
	UEvent* EventToFire = nullptr;
	
	for (auto KV : Events)
	{
		UEvent* Event = KV.Value;
		if (!Event->CanBeTriggered())
		{
			continue;
		}
		const float EventProbability = Event->GetTriggerProbability();
		const float ChanceToTrigger = FMath::FRandRange<double>(0.f, TriggerChanceModifier);
		if (EventProbability >= ChanceToTrigger)
		{
			EventToFire = Event;
			break;
		}
	}
	return EventToFire;
}

UEvent* UEventSubsystem::AddForcedEvent(TSubclassOf<UEvent> Event)
{
	ForcedEvents.Add(Event);
	return StaticEvents.Contains(Event) ? StaticEvents[Event] : nullptr;
}

UEvent* UEventSubsystem::GetEvent(const TSubclassOf<UEvent> EventClass) const
{
	UEvent* FoundEvent = Events.FindRef(EventClass);
	if (!FoundEvent)
	{
		FoundEvent = StaticEvents.FindRef(EventClass);
	}
	return FoundEvent;
}

bool UEventSubsystem::IsEventForced(TSubclassOf<UEvent> EventClass) const
{
	return ForcedEvents.Contains(EventClass);
}

UEvent* UEventSubsystem::PickEventToTrigger(const float TriggerChanceModifier)
{
	if (!ForcedEvents.IsEmpty())
	{
		return StaticEvents.FindRef(ForcedEvents.Pop());
	}
	return SampleRandomEvents(TriggerChanceModifier);
}

void UEventSubsystem::HandleEvents(const FDateTime CurrentTime)
{
	const float HoursSinceLastEvent = (CurrentTime - LastAnyEventFiredDate).GetHours();
	const float Probability = 1.f * (HoursSinceLastEvent > 1.f) ? FMath::Pow<double>(0.5f, HoursSinceLastEvent * 0.2f) : 1.f;
	
	UEvent* EventToFire = PickEventToTrigger(Probability);
	if (EventToFire)
	{
		LastAnyEventFiredDate = CurrentTime;
		TriggerEvent(*EventToFire);
	}
#if ENABLE_DRAW_DEBUG
	if (CVarEventSystem.GetValueOnAnyThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, EventToFire ? TEXT("Event Fired: ") + EventToFire->EventName.ToString() + TEXT("Event probability:") : TEXT("Event not fired"));
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString(TEXT("Current Take: ") + FString::SanitizeFloat(Probability)));
	}
#endif
}

void UEventSubsystem::TriggerEvent(UEvent& EventToTrigger)
{
	EventToTrigger.Trigger();
	
	OnEventTriggered.Broadcast(&EventToTrigger);
}
