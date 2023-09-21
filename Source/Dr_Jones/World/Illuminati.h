// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Clock.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "Illuminati.generated.h"

class UQuestSystemComponent;

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWorldClockTickDelegate, FWorldClockTime, ClockTime, bool, bInitialTick);

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

UCLASS(Blueprintable, ClassGroup = "DrJones", HideCategories = (Replication, Actor, Physics, Cooking))
class DR_JONES_API AIlluminati : public AInfo
{
	GENERATED_BODY()

public:
	AIlluminati();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	FClock& GetClock() { return Clock; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Clock")
	void OnClockTickEvent(const FWorldClockTime& ClockTime, bool bInitialTick);

	UFUNCTION(BlueprintPure, Category = "Clock")
	static float GetClockSecondsPerRealSecond();

	UFUNCTION(BlueprintCallable, Category = "Clock", meta = (AutoCreateRefTerm = "WorldClockTime"))
	void SetCurrentClockTime(const FWorldClockTime& WorldClockTime);

	UFUNCTION(BlueprintPure, Category = "Clock")
	FWorldClockTime GetCurrentClockTime() const;

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Time, Event"))
	FWorldEventHandle ScheduleEventOnce(const FWorldClockTime& Time, const FWorldEventDelegate& Event);

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Schedule, Event"))
	FWorldEventHandle ScheduleEvent(const FWorldEventSchedule& Schedule, const FWorldEventDelegate& Event);

	UFUNCTION(BlueprintCallable, Category = "World Event", meta = (AutoCreateRefTerm = "Schedule, Event"))
	FWorldEventHandle ScheduleEventWithRule(const FWorldEventSchedule& Schedule, UWorldEventRule* EventRule, const FWorldEventDelegate& Event);

public:
	UPROPERTY(BlueprintAssignable, Category = "Clock")
	FWorldClockTickDelegate ClockTickDelegate;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UQuestSystemComponent> QuestSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clock")
	FDateTime InitialTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clock")
	uint8 bDisableFirstClockTick : 1;

private:
	FClock Clock = FClock(*this);
	TMap<FWorldEventHandle, TArray<FClockTaskHandle>> WorldEventCollection;
};
