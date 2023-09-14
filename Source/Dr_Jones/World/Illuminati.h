// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Clock.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "Illuminati.generated.h"

USTRUCT(BlueprintType)
struct FWorldClockTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, ClampMax = 23, UIMax = 23))
	int32 Hours = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, ClampMax = 59, UIMax = 59))
	int32 Minutes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, ClampMax = 59, UIMax = 59))
	int32 Seconds = 0;

	FClockTime ToClockTime() const;
	void InitFromClockTime(const FClockTime& ClockTime);
	static FWorldClockTime MakeFromClockTime(const FClockTime& ClockTime);
};

USTRUCT(BlueprintType)
struct FWorldEventSchedule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWorldClockTime> ScheduleTimes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeviationMinutes = 10.0f;
};

UCLASS(Blueprintable)
class DR_JONES_API UWorldEventRule : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool CanEventExecute() const;
};

DECLARE_DYNAMIC_DELEGATE(FWorldEventDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldClockTickDelegate, FWorldClockTime, ClockTime);

USTRUCT(BlueprintType)
struct FWorldEventHandle
{
	GENERATED_BODY()

	uint32 Key;
	inline static uint32 KeyCounter = 0;

	FWorldEventHandle() : Key(KeyCounter++) {}

	bool operator==(const FWorldEventHandle& Other) const { return Key == Other.Key; }
};

inline int32 GetTypeHash(const FWorldEventHandle& WorldEventHandle)
{
	return GetTypeHash(WorldEventHandle.Key);
}

UCLASS(Blueprintable, ClassGroup = "DrJones")
class DR_JONES_API AIlluminati : public AInfo
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	FClock& GetClock() { return Clock; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Clock")
	void OnClockTickEvent(const FWorldClockTime& ClockTime);

	UFUNCTION(BlueprintPure, Category = "Clock")
	static float GetClockSecondsPerRealSecond();

	UFUNCTION(BlueprintPure, Category = "Clock")
	FWorldClockTime GetCurrentClockTime() const;

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Time, Event"))
	FWorldEventHandle ScheduleEventOnce(const FWorldClockTime& Time, const FWorldEventDelegate& Event);

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Schedule, Event"))
	FWorldEventHandle ScheduleEvent(const FWorldEventSchedule& Schedule, const FWorldEventDelegate& Event);

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Schedule, Event"))
	FWorldEventHandle ScheduleEventWithRule(const FWorldEventSchedule& Schedule, UWorldEventRule* EventRule, const FWorldEventDelegate& Event);

	UPROPERTY(BlueprintAssignable, Category = "Clock")
	FWorldClockTickDelegate ClockTickDelegate;

private:
	FClock Clock = FClock(*this);
	TMap<FWorldEventHandle, TArray<FClockTaskHandle>> WorldEventCollection;
};

FORCEINLINE FWorldClockTime AIlluminati::GetCurrentClockTime() const
{
	return FWorldClockTime::MakeFromClockTime(Clock.GetTime());
}
