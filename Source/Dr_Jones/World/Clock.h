// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Clock.generated.h"

static TAutoConsoleVariable CVarTime(TEXT("NS.Time"), 0, TEXT("Shows current date and time"));

UCLASS(meta = (Instanced))
class DR_JONES_API UClock : public UObject
{
	GENERATED_BODY()

public:
	void InitializeClock(FDateTime StartTime);
	void TickClock();
	
	void SetClockPaused(bool bPause = true);

	UFUNCTION(BlueprintPure)
	FDateTime GetCurrentTime() const { return CurrentTime; }
	
	void SetTime(FDateTime NewTime);
	FDateTime SkipTime(FTimespan TimeToSkip);
	FTimespan SkipTime(FDateTime TargetTime);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnClockNativeTick, FDateTime);
	FOnClockNativeTick OnClockNativeTick;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClockTick, FDateTime, CurrentDate);
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Clock")
	FOnClockTick OnClockTick;
	
public:
	UPROPERTY(SaveGame)
	FDateTime CurrentTime;

	UPROPERTY(EditAnywhere)
	FTimespan TimeToApplyPerSecond = FTimespan(0,0,30);
	
	FTimerHandle ClockTimer;
};