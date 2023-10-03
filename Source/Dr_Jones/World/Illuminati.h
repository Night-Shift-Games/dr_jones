// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Clock.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "Illuminati.generated.h"

class USoundCue;
class UQuestSystemComponent;

static TAutoConsoleVariable CVarIlluminatiDebug(
	TEXT("NS.Illuminati.Debug"),
	false,
	TEXT("Enable Illuminati debug logging."),
	ECVF_Cheat
);

USTRUCT(BlueprintType)
struct DR_JONES_API FWorldClockTimeOffset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimespan Timespan;

	int32 ToSeconds() const;
};

USTRUCT(BlueprintType)
struct DR_JONES_API FWorldClockTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DateTime;

	FClockTime ToClockTime() const;
	void InitFromClockTime(const FClockTime& ClockTime);
	static FWorldClockTime MakeFromClockTime(const FClockTime& ClockTime);
};

// TODO: Refactor this so it is possible to specify if the events are reoccurring every day / hour etc.
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

USTRUCT(BlueprintType, Category = "Illuminati")
struct FIlluminatiGlobalEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Event")
	TObjectPtr<USoundBase> RadioSound;
};

DECLARE_DYNAMIC_DELEGATE(FWorldEventDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWorldClockTickDelegate, FWorldClockTime, ClockTime, bool, bInitialTick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalEventReceivedDelegate, const FIlluminatiGlobalEvent&, GlobalEvent);

class FIlluminatiDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWorldClockTickDelegate, FWorldClockTime, bool /*bInitialTick*/);
	static inline FOnWorldClockTickDelegate OnWorldClockTick;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalEventReceivedDelegate, const FIlluminatiGlobalEvent&);
	static inline FOnGlobalEventReceivedDelegate OnGlobalEventReceived;
};

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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "World Event", meta = (AutoCreateRefTerm = "GlobalEvent"))
	void PostGlobalEvent(const FIlluminatiGlobalEvent& GlobalEvent) const;

	UFUNCTION(BlueprintPure, Category = "Illuminati", meta = (WorldContext = "WorldContextObject"))
	static AIlluminati* GetIlluminatiInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Illuminati", meta = (WorldContext = "WorldContextObject"))
	static UQuestSystemComponent* GetQuestSystemInstance(const UObject* WorldContextObject);

public:
	UPROPERTY(BlueprintAssignable, Category = "Clock")
	FWorldClockTickDelegate ClockTickDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Global Event")
	FOnGlobalEventReceivedDelegate OnGlobalEventReceived;

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
