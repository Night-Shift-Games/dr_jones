// Property of Night Shift Games, all rights reserved.

#include "Clock.h"
#include "Engine/World.h"
#include "Utilities/NightShiftSettings.h"

void UClock::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeDataFromSettings();
}

void UClock::InitializeDataFromSettings()
{
	TimeToApplyPerSecond = GetDefault<UNightShiftSettings>()->TimeToApplyPerSecond;
	CurrentTime = GetDefault<UNightShiftSettings>()->InitialTime;
}

void UClock::StartClock()
{
	OnClockNativeTick.AddWeakLambda(this, [&](FDateTime Time){ OnClockTick.Broadcast(Time); });
	GetWorld()->GetTimerManager().SetTimer(ClockTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		TickClock();
	}), 1.f, true);
	OnTimeSkipNative.AddWeakLambda(this, [&](FDateTime TimeBeforeSkip, FTimespan SkippedTime) { OnTimeSkip.Broadcast(TimeBeforeSkip, SkippedTime); });
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
	FDateTime TimeBeforeSkip = GetCurrentTime();
	CurrentTime += TimeToSkip;
	OnTimeSkipNative.Broadcast(TimeBeforeSkip, TimeToSkip);
	
	return CurrentTime;
}

FTimespan UClock::SkipTime(const FDateTime TargetTime)
{
	const FTimespan DeltaTime = CurrentTime - TargetTime;
	SkipTime(DeltaTime);
	return DeltaTime;
}
