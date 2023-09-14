// Property of Night Shift Games, all rights reserved.

#include "Clock.h"
#include "Engine/World.h"

FClockTime::FClockTime(uint64 Seconds)
	: TimeSeconds(Seconds)
{
}

FClockTime::FClockTime(uint64 Hours, uint64 Minutes, uint64 Seconds)
{
	check(Hours < 24);
	check(Minutes < 60);
	check(Seconds < 60);

	TimeSeconds = Hours * 3600 + Minutes * 60 + Seconds;
}

FClockTime::FClockTime(const FClockTimeComponents& Components)
	: FClockTime(Components.Hours, Components.Minutes, Components.Seconds)
{
}

FClockTimeComponents FClockTime::GetComponents() const
{
	FClockTimeComponents Components;
	Components.Hours = TimeSeconds / 3600;
	Components.Minutes = (TimeSeconds / 60) % 60;
	Components.Seconds = TimeSeconds % 60;
	return Components;
}

FClockTime FClockTime::ConvertToTimeZoneFromUTC(FClockTimeZone TimeZone) const
{
	return FClockTime(TimeSeconds + TimeZone.GetHourOffset() * 3600);
}

bool FClockTime::IsConsideredEqualDuringClockUpdate(const FClockTime& Other, uint64 UpdateInterval) const
{
	const int64 MinimumTimeToConsiderSameExclusive = TimeSeconds - UpdateInterval;
	return static_cast<int64>(Other.TimeSeconds) > MinimumTimeToConsiderSameExclusive && Other.TimeSeconds <= TimeSeconds;
}

FClockTime& FClockTime::operator+=(uint64 Seconds)
{
	TimeSeconds = (TimeSeconds + Seconds) % MaxTime;
	return *this;
}

bool FClockTime::operator==(const FClockTime& ClockTime) const
{
	return TimeSeconds == ClockTime.TimeSeconds;
}

FClock::FClock()
{
}

FClock::FClock(UObject& OuterObject)
	: Outer(&OuterObject)
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
