// Property of Night Shift Games, all rights reserved.

#include "Illuminati.h"

#include "Managment/Dr_JonesGameModeBase.h"
#include "Quest/QuestSystem.h"

int32 FWorldClockTimeOffset::ToSeconds() const
{
	return Timespan.GetTicks() / ETimespan::TicksPerSecond;
}

FClockTime FWorldClockTime::ToClockTime() const
{
	return FClockTime(DateTime);
}

void FWorldClockTime::InitFromClockTime(const FClockTime& ClockTime)
{
	DateTime = ClockTime.GetDateTime();
}

FWorldClockTime FWorldClockTime::MakeFromClockTime(const FClockTime& ClockTime)
{
	FWorldClockTime WorldClockTime;
	WorldClockTime.InitFromClockTime(ClockTime);
	return WorldClockTime;
}

bool UWorldEventRule::CanEventExecute_Implementation() const
{
	// If not specified, the event can always execute
	return true;
}

AIlluminati::AIlluminati()
{
	QuestSystemComponent = CreateDefaultSubobject<UQuestSystemComponent>(TEXT("QuestSystem"));
}

void AIlluminati::BeginPlay()
{
	Super::BeginPlay();

	Clock.SetWorldContext(*GetWorld());
	Clock.ClockTickDelegate.BindWeakLambda(this, [this](const FClockTime& ClockTime)
	{
		const FWorldClockTime WorldClockTime = FWorldClockTime::MakeFromClockTime(ClockTime);
		ClockTickDelegate.Broadcast(WorldClockTime, Clock.IsInitializeTick());
		OnClockTickEvent(WorldClockTime, Clock.IsInitializeTick());
	});
	Clock.SetTime(FClockTime(InitialTime));
	Clock.Start(bDisableFirstClockTick);
}

void AIlluminati::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

float AIlluminati::GetClockSecondsPerRealSecond()
{
	return FClock::ClockSecondsPerRealSecond;
}

void AIlluminati::SetCurrentClockTime(const FWorldClockTime& WorldClockTime)
{
	Clock.SetTime(WorldClockTime.ToClockTime());
}

FWorldClockTime AIlluminati::GetCurrentClockTime() const
{
	return FWorldClockTime::MakeFromClockTime(Clock.GetTime());
}

FWorldEventHandle AIlluminati::ScheduleEventOnce(const FWorldClockTime& Time, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTaskOnce(Time.ToClockTime(),
		FClockTickDelegate::CreateWeakLambda(this, [Event](const FClockTime& ClockTime)
		{
			(void)Event.ExecuteIfBound();
		}));
	ClockTaskHandles.Add(ClockTaskHandle);
	return Handle;
}

FWorldEventHandle AIlluminati::ScheduleEvent(const FWorldEventSchedule& Schedule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTask(ClockTime,
			FClockTickDelegate::CreateWeakLambda(this, [Event](const FClockTime& ClockTime)
			{
				(void)Event.ExecuteIfBound();
			}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}

FWorldEventHandle AIlluminati::ScheduleEventWithRule(const FWorldEventSchedule& Schedule, UWorldEventRule* EventRule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const FClockTaskHandle ClockTaskHandle = Clock.ScheduleTask(ClockTime,
			FClockTickDelegate::CreateWeakLambda(this, [Event, EventRule](const FClockTime& ClockTime)
			{
				if (EventRule->CanEventExecute())
				{
					(void)Event.ExecuteIfBound();
				}
			}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}

AIlluminati* AIlluminati::GetIlluminatiInstance(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	const ADr_JonesGameModeBase* GameMode = World->GetAuthGameMode<ADr_JonesGameModeBase>();
	if (!ensureAlwaysMsgf(GameMode, TEXT("Tried to get Illuminati instance, but an incorrect game mode is active.")))
	{
		return nullptr;
	}

	return GameMode->GetIlluminati();
}

UQuestSystemComponent* AIlluminati::GetQuestSystemInstance(const UObject* WorldContextObject)
{
	const AIlluminati* Illuminati = GetIlluminatiInstance(WorldContextObject);
	if (!Illuminati)
	{
		return nullptr;
	}

	return Illuminati->QuestSystemComponent;
}
