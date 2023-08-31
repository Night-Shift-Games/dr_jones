// Property of Night Shift Games, all rights reserved.

#include "Illuminati.h"

FClockTime FWorldClockTime::ToClockTime() const
{
	return FClockTime(Hours, Minutes, Seconds);
}

void FWorldClockTime::InitFromClockTime(const FClockTime& ClockTime)
{
	const FClockTimeComponents Components = ClockTime.GetComponents();
	Hours = Components.Hours;
	Minutes = Components.Minutes;
	Seconds = Components.Seconds;
}

FWorldClockTime FWorldClockTime::MakeFromClockTime(const FClockTime& ClockTime)
{
	FWorldClockTime WorldClockTime;
	WorldClockTime.InitFromClockTime(ClockTime);
	return WorldClockTime;
}

bool UWorldEventRule::CanEventExecute_Implementation() const
{
	// If not specified, the event can always execute
	return true;
}

void UIlluminati::Initialize(UWorld& World)
{
	WorldContext = &World;

	Clock.SetWorldContext(World);
	Clock.Start();

	BeginPlay();
}

FWorldEventHandle UIlluminati::ScheduleEventOnce(const FWorldClockTime& Time, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTaskOnce(Time.ToClockTime(),
		FClockTaskDelegate::CreateWeakLambda(this, [Event](FClockTime& ClockTime)
		{
			(void)Event.ExecuteIfBound();
		}));
	ClockTaskHandles.Add(ClockTaskHandle);
	return Handle;
}

FWorldEventHandle UIlluminati::ScheduleEvent(const FWorldEventSchedule& Schedule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTask(ClockTime,
			FClockTaskDelegate::CreateWeakLambda(this, [Event](FClockTime& ClockTime)
			{
				(void)Event.ExecuteIfBound();
			}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}

FWorldEventHandle UIlluminati::ScheduleEventWithRule(const FWorldEventSchedule& Schedule, UWorldEventRule* EventRule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTask(ClockTime,
			FClockTaskDelegate::CreateWeakLambda(this, [Event, EventRule](FClockTime& ClockTime)
			{
				if (EventRule->CanEventExecute())
				{
					(void)Event.ExecuteIfBound();
				}
			}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}
