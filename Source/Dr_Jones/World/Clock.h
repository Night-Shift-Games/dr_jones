// Property of Night Shift Games, all rights reserved.

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
	explicit FClockTimeZone(int64 TimeZoneHourOffset)
		: HourOffset(TimeZoneHourOffset)
	{
		checkf(HourOffset >= -11 && HourOffset <= 14, TEXT("Non-existing time zone."));
	}

	int64 GetHourOffset() const { return HourOffset; }

private:
	int64 HourOffset;
};

class DR_JONES_API FClockTime
{
public:
	static constexpr inline uint64 MaxTime = 60 * 60 * 24;

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

DECLARE_DELEGATE_OneParam(FClockTickDelegate, const FClockTime&)

struct FClockScheduledTask
{
	FClockTime TimeToExecuteAt;
	FClockTickDelegate TaskDelegate;
	bool bRemoveAfterExecute = false;
};

class DR_JONES_API FClock
{
public:
	static constexpr inline uint64 ClockSecondsPerRealSecond = 20;

	FClock(); // <-- UE internal
	explicit FClock(UObject& OuterObject);

	void SetWorldContext(UWorld& World);

	void Start(bool bWithFirstTickDisabled = false);
	void Stop();
	void Pause() const;
	void UnPause() const;

	void SetTime(FClockTime Time);
	FClockTime GetTime() const;
	void SkipTime(int32 SecondsToSkip);

	FClockTaskHandle ScheduleTask(FClockTime Time, const FClockTickDelegate& TaskDelegate);
	FClockTaskHandle ScheduleTaskOnce(FClockTime Time, const FClockTickDelegate& TaskDelegate);
	void RemoveTask(const FClockTaskHandle& Handle);

	bool IsInitializeTick() const { return bInitializeTick; }

protected:
	void Update();

public:
	FClockTickDelegate ClockTickDelegate;

private:
	TMap<FClockTaskHandle::KeyType, FClockScheduledTask> ScheduledTasks;
	TWeakObjectPtr<UWorld> WorldContext;
	TWeakObjectPtr<> Outer;
	FTimerHandle ClockTimerHandle;
	FClockTime CurrentTime;
	bool bInitializeTick;
};

FORCEINLINE FClockTime FClock::GetTime() const
{
	return CurrentTime;
}
