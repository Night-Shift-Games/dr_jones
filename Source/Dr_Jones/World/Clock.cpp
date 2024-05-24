﻿// Property of Night Shift Games, all rights reserved.

#include "Clock.h"
#include "Engine/World.h"

void UClock::InitializeClock(const FDateTime StartTime)
{
	OnClockNativeTick.AddWeakLambda(this, [&](FDateTime Time){ OnClockTick.Broadcast(Time); });
	CurrentTime = StartTime;
	GetWorld()->GetTimerManager().SetTimer(ClockTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		TickClock();
	}), 1.f, true);
}

void UClock::TickClock()
{
	CurrentTime += TimeToApplyPerSecond;
	OnClockNativeTick.Broadcast(CurrentTime);
#if ENABLE_DRAW_DEBUG
	if (CVarTime.GetValueOnAnyThread())
	{
		GEngine->AddOnScreenDebugMessage(2400, 1.f, FColor::Red, CurrentTime.ToString());
	}
#endif
}

void UClock::SetClockPaused(bool bPause)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (bPause && !TimerManager.IsTimerPaused(ClockTimer))
	{
		TimerManager.PauseTimer(ClockTimer);
	}
	else if (!bPause && TimerManager.IsTimerPaused(ClockTimer))
	{
		TimerManager.UnPauseTimer(ClockTimer);
	}
}

void UClock::SetTime(const FDateTime NewTime)
{
	CurrentTime = NewTime;
}

FDateTime UClock::SkipTime(const FTimespan TimeToSkip)
{
	return CurrentTime += TimeToSkip;
}

FTimespan UClock::SkipTime(const FDateTime TargetTime)
{
	const FTimespan DeltaTime = CurrentTime - TargetTime;
	SkipTime(DeltaTime);
	return DeltaTime;
}
