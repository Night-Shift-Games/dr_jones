// Property of Night Shift Games, all rights reserved.

#include "Clock.h"
#include "Engine/World.h"

FClockTime::FClockTime(const FDateTime& InDateTime)
	: DateTime(InDateTime)
{
}

FClockTimeComponents FClockTime::GetComponents() const
{
	FClockTimeComponents Components;
	Components.Hours = DateTime.GetHour();
	Components.Minutes = DateTime.GetMinute();
	Components.Seconds = DateTime.GetSecond();
	return Components;
}

FClockTime FClockTime::ConvertToTimeZoneFromUTC(FClockTimeZone TimeZone) const
{
	return FClockTime(DateTime + FTimespan(TimeZone.GetHourOffset(), 0, 0));
}

bool FClockTime::IsConsideredEqualDuringClockUpdate(const FClockTime& Other, uint64 UpdateIntervalSeconds) const
{
	const FDateTime MinimumDateTimeToConsiderSameExclusive = DateTime - FTimespan(UpdateIntervalSeconds * ETimespan::TicksPerSecond);
	return Other.DateTime > MinimumDateTimeToConsiderSameExclusive && Other.DateTime <= DateTime;
}

FClockTime& FClockTime::operator+=(uint64 Seconds)
{
	DateTime += FTimespan(Seconds * ETimespan::TicksPerSecond);
	return *this;
}

FClockTime& FClockTime::operator+=(FTimespan Timespan)
{
	DateTime += Timespan;
	return *this;
}

bool FClockTime::operator==(const FClockTime& ClockTime) const
{
	return DateTime == ClockTime.DateTime;
}

FClock::FClock()
	: bInitializeTick(true)
{
}

FClock::FClock(UObject& OuterObject)
	: Outer(&OuterObject)
	, bInitializeTick(true)
{
}

void FClock::SetWorldContext(UWorld& World)
{
	checkf(!WorldContext.IsValid(), TEXT("WorldContext already set."));
	WorldContext = &World;
}

void FClock::Start(bool bWithFirstTickDisabled)
{
	check(WorldContext.IsValid());

	if (ClockTimerHandle.IsValid())
	{
		return;
	}

	if (!bWithFirstTickDisabled)
	{
		WorldContext->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(Outer.Get(), [this]
		{
			Update();
		}));
	}
	WorldContext->GetTimerManager().SetTimer(ClockTimerHandle, FTimerDelegate::CreateWeakLambda(Outer.Get(), [this]
	{
		bInitializeTick = false;
		Update();
	}), 1.0f, true);
}

void FClock::Stop()
{
	check(WorldContext.IsValid());

	if (!ClockTimerHandle.IsValid())
	{
		return;
	}

	WorldContext->GetTimerManager().ClearTimer(ClockTimerHandle);
}

void FClock::Pause() const
{
	check(WorldContext.IsValid());

	if (!ClockTimerHandle.IsValid())
	{
		return;
	}

	WorldContext->GetTimerManager().PauseTimer(ClockTimerHandle);
}

void FClock::UnPause() const
{
	check(WorldContext.IsValid());

	if (!ClockTimerHandle.IsValid())
	{
		return;
	}

	WorldContext->GetTimerManager().UnPauseTimer(ClockTimerHandle);
}

void FClock::SetTime(FClockTime Time)
{
	CurrentTime = Time;
}

void FClock::SkipTime(FTimespan TimeToSkip)
{
	FClockTime Time = GetTime();
	Time += TimeToSkip;
	SetTime(Time);
}

FClockTaskHandle FClock::ScheduleTask(FClockTime Time, const FClockTickDelegate& TaskDelegate)
{
	FClockScheduledTask Task;
	Task.TimeToExecuteAt = Time;
	Task.TaskDelegate = TaskDelegate;

	FClockTaskHandle Handle;
	ScheduledTasks.Emplace(Handle.Key, MoveTemp(Task));

	return Handle;
}

FClockTaskHandle FClock::ScheduleTaskOnce(FClockTime Time, const FClockTickDelegate& TaskDelegate)
{
	FClockScheduledTask Task;
	Task.TimeToExecuteAt = Time;
	Task.TaskDelegate = TaskDelegate;
	Task.bRemoveAfterExecute = true;

	FClockTaskHandle Handle;
	ScheduledTasks.Emplace(Handle.Key, MoveTemp(Task));

	return Handle;
}

void FClock::RemoveTask(const FClockTaskHandle& Handle)
{
	ScheduledTasks.Remove(Handle.Key);
}

void FClock::Update()
{
	for (decltype(ScheduledTasks)::TIterator It = ScheduledTasks.CreateIterator(); It; ++It)
	{
		auto& [HandleKey, Task] = *It;

		if (Task.TimeToExecuteAt.IsConsideredEqualDuringClockUpdate(CurrentTime, ClockSecondsPerRealSecond))
		{
			check(Task.TaskDelegate.IsBound());
			Task.TaskDelegate.Execute(CurrentTime);
			if (Task.bRemoveAfterExecute)
			{
				ScheduledTasks.Remove(HandleKey);
			}
		}
	}

	(void)ClockTickDelegate.ExecuteIfBound(CurrentTime);

	CurrentTime += ClockSecondsPerRealSecond;
}
