// Property of Night Shift Games, all rights reserved.

#include "EventSubsystem.h"

#include "Utilities/NightShiftSettings.h"
#include "World/Clock.h"

void UEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<TSubclassOf<UEvent>> EventsToCreate = GetDefault<UNightShiftSettings>()->Events;

	for (TSubclassOf<UEvent> EventClass : EventsToCreate)
	{
		UEvent* NewEvent = NewObject<UEvent>(this, EventClass);
		Events.Add(EventClass, TStrongObjectPtr(NewEvent));
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
	
	Super::Deinitialize();
}

void UEventSubsystem::HandleEvents(const FDateTime CurrentTime)
{
	int32 TakeRange = 100.f / ((CurrentTime - LastEventFiredDate).GetHours() + 1);
#if ENABLE_DRAW_DEBUG
	if (CVarEventSystem.GetValueOnAnyThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Sampling Event"));
	}
#endif
	UEvent* EventToFire = nullptr;
	for (auto KV : Events)
	{
		UEvent* Event = KV.Value.Get();
		if (!Event->CanBeTriggered())
		{
			continue;
		}
		const float EventProbability = Event->ProbabilityOfEvent * 100.f;
		const float Take = FMath::RandRange(0, TakeRange);
		if (EventProbability >= Take)
		{
			EventToFire = Event;
			break;
		}
	}
	if (EventToFire)
	{
		LastEventFiredDate = CurrentTime;
		TriggerEvent(*EventToFire);
	}
#if ENABLE_DRAW_DEBUG
	if (CVarEventSystem.GetValueOnAnyThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, EventToFire ? EventToFire->EventName.ToString() : TEXT("Event not fired"));
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString(TEXT("Current Take:") + FString::FromInt(TakeRange)));
	}
#endif
}

void UEventSubsystem::TriggerEvent(UEvent& EventToTrigger)
{
	EventToTrigger.Trigger();
	
	OnEventTriggered.Broadcast(&EventToTrigger);
}
