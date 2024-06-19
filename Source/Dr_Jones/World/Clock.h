// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Clock.generated.h"

inline TAutoConsoleVariable CVarTime(TEXT("NS.Time"), 0, TEXT("Shows current date and time"));

UCLASS()
class DR_JONES_API UClock : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void InitializeDataFromSettings();

	void StartClock();
	void TickClock();
	void SetClockPaused(bool bPause = true);

	UFUNCTION(BlueprintPure, Category = "DrJones|Clock")
	FDateTime GetCurrentTime() const { return CurrentTime; }

	UFUNCTION(BlueprintCallable, Category = "DrJones|Clock")
	void SetTime(const FDateTime NewTime);
	
	FDateTime SkipTime(const FTimespan TimeToSkip);
	FTimespan SkipTime(const FDateTime TargetTime);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClockTick, FDateTime, CurrentDate);
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Clock")
	FOnClockTick OnClockTick;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnClockNativeTick, FDateTime);
	FOnClockNativeTick OnClockNativeTick;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeSkipDynamic, FDateTime, DateBeforeTimeSkip, FTimespan, TimeSkipped);
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Clock")
	FOnTimeSkipDynamic OnTimeSkip;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTimeSkipNative, FDateTime, FTimespan);
	FOnTimeSkipNative OnTimeSkipNative;
public:
	UPROPERTY(SaveGame)
	FDateTime CurrentTime;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FTimespan TimeToApplyPerSecond = FTimespan(0,1,0);
	
	FTimerHandle ClockTimer;
};