﻿// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

enum EClockTaskNoInit {};

class FClockTaskHandle
{
public:
	using KeyType = uint32;

	FClockTaskHandle()
		: Key(KeyCounter++)
	{
	}

	explicit FClockTaskHandle(KeyType ExistingKey)
		: Key(ExistingKey)
	{
	}

	explicit FClockTaskHandle(EClockTaskNoInit)
	{
	}

private:
	KeyType Key;
	inline static KeyType KeyCounter = 0;

	friend class FClock;
};

struct FClockTimeComponents
{
	uint64 Hours;
	uint64 Minutes;
	uint64 Seconds;
};

class FClockTimeZone
{
public:
	explicit FClockTimeZone(uint64 TimeZoneHourOffset)
		: HourOffset(TimeZoneHourOffset)
	{
		checkf(HourOffset >= -11 && HourOffset <= 14, TEXT("Non-existing time zone."));
	}

	uint64 GetHourOffset() const { return HourOffset; }

private:
	uint64 HourOffset;
};

class DR_JONES_API FClockTime
{
public:
	static constexpr inline uint64 MaxTime = 60 * 60 * 60;

	FClockTime()
		: TimeSeconds(0)
	{}

	FClockTime(uint64 Seconds);
	FClockTime(uint64 Hours, uint64 Minutes, uint64 Seconds);
	FClockTime(const FClockTimeComponents& Components);
	FClockTimeComponents GetComponents() const;

	FClockTime ConvertToTimeZoneFromUTC(FClockTimeZone TimeZone) const;

	bool IsConsideredEqualDuringClockUpdate(const FClockTime& Other, uint64 UpdateInterval) const;

	FClockTime& operator+=(uint64 Seconds);
	bool operator==(const FClockTime& Other) const;

private:
	uint64 TimeSeconds;
};

DECLARE_DELEGATE_OneParam(FClockTaskDelegate, FClockTime&)

struct FClockScheduledTask
{
	FClockTime TimeToExecuteAt;
	FClockTaskDelegate TaskDelegate;
	bool bRemoveAfterExecute = false;
};

class DR_JONES_API FClock
{
public:
	static constexpr inline uint64 ClockSecondsPerRealSecond = 20;

	FClock(); // <-- UE internal
	explicit FClock(UObject& OuterObject);

	void SetWorldContext(UWorld& World);

	void Start();
	void Stop();
	void Pause() const;
	void UnPause() const;

	void SetTime(FClockTime Time);
	FClockTime GetTime() const;

	FClockTaskHandle ScheduleTask(FClockTime Time, const FClockTaskDelegate& TaskDelegate);
	FClockTaskHandle ScheduleTaskOnce(FClockTime Time, const FClockTaskDelegate& TaskDelegate);
	void RemoveTask(const FClockTaskHandle& Handle);

protected:
	void Update();

private:
	TMap<FClockTaskHandle::KeyType, FClockScheduledTask> ScheduledTasks;
	TWeakObjectPtr<UWorld> WorldContext;
	TWeakObjectPtr<> Outer;
	FTimerHandle ClockTimerHandle;
	FClockTime CurrentTime;
};

FORCEINLINE FClockTime FClock::GetTime() const
{
	return CurrentTime;
}