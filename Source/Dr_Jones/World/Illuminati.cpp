// Property of Night Shift Games, all rights reserved.

#include "Illuminati.h"

#include "Managment/Dr_JonesGameModeBase.h"
#include "Quest/QuestSystem.h"

int32 FWorldClockTimeOffset::ToSeconds() const
{
	return Timespan.GetTicks() / ETimespan::TicksPerSecond;
}

DrJones::Deprecated::FClockTime FWorldClockTime::ToClockTime() const
{
	return DrJones::Deprecated::FClockTime(DateTime);
}

void FWorldClockTime::InitFromClockTime(const DrJones::Deprecated::FClockTime& ClockTime)
{
	DateTime = ClockTime.GetDateTime();
}

FWorldClockTime FWorldClockTime::MakeFromClockTime(const DrJones::Deprecated::FClockTime& ClockTime)
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
	Clock = CreateDefaultSubobject<UClock>(TEXT("WorldClock"));
}

void AIlluminati::BeginPlay()
{
	Super::BeginPlay();

	Clock->InitializeClock(InitialTime);
	
	DEPRECATED_Clock.SetWorldContext(*GetWorld());
	DEPRECATED_Clock.ClockTickDelegate.BindWeakLambda(this, [this](const DrJones::Deprecated::FClockTime& ClockTime)
	{
		const FWorldClockTime WorldClockTime = FWorldClockTime::MakeFromClockTime(ClockTime);
		FIlluminatiDelegates::OnWorldClockTick.Broadcast(WorldClockTime, DEPRECATED_Clock.IsInitializeTick());
		ClockTickDelegate.Broadcast(WorldClockTime, DEPRECATED_Clock.IsInitializeTick());
		OnClockTickEvent(WorldClockTime, DEPRECATED_Clock.IsInitializeTick());
	});
	DEPRECATED_Clock.Start(bDisableFirstClockTick);
}

void AIlluminati::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

float AIlluminati::GetClockSecondsPerRealSecond()
{
	return DrJones::Deprecated::FClock::ClockSecondsPerRealSecond;
}

void AIlluminati::SetCurrentClockTime(const FWorldClockTime& WorldClockTime)
{
	DEPRECATED_Clock.SetTime(WorldClockTime.ToClockTime());
}

FWorldClockTime AIlluminati::GetCurrentClockTime() const
{
	return FWorldClockTime::MakeFromClockTime(DEPRECATED_Clock.GetTime());
}

FWorldEventHandle AIlluminati::ScheduleEventOnce(const FWorldClockTime& Time, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<DrJones::Deprecated::FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	const auto& ClockTaskHandle = DEPRECATED_Clock.ScheduleTaskOnce(Time.ToClockTime(), DrJones::Deprecated::FClockTickDelegate::CreateWeakLambda(this, [Event](const DrJones::Deprecated::FClockTime& ClockTime)
	{
		Event.ExecuteIfBound();
	}));
	ClockTaskHandles.Add(ClockTaskHandle);
	return Handle;
}

FWorldEventHandle AIlluminati::ScheduleEvent(const FWorldEventSchedule& Schedule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<DrJones::Deprecated::FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		DrJones::Deprecated::FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const auto ClockTaskHandle = DEPRECATED_Clock.ScheduleTask(ClockTime, DrJones::Deprecated::FClockTickDelegate::CreateWeakLambda(this, [Event](const DrJones::Deprecated::FClockTime& ClockTime)
		{
			Event.ExecuteIfBound();
		}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}

FWorldEventHandle AIlluminati::ScheduleEventWithRule(const FWorldEventSchedule& Schedule, UWorldEventRule* EventRule, const FWorldEventDelegate& Event)
{
	const FWorldEventHandle Handle;
	TArray<DrJones::Deprecated::FClockTaskHandle>& ClockTaskHandles = WorldEventCollection.Add(Handle);
	ClockTaskHandles.Reserve(Schedule.ScheduleTimes.Num());
	for (const FWorldClockTime& Time : Schedule.ScheduleTimes)
	{
		DrJones::Deprecated::FClockTime ClockTime = Time.ToClockTime();
		ClockTime += (FMath::RandRange(0.0f, Schedule.DeviationMinutes) - Schedule.DeviationMinutes / 2.0f) * 60;
		const DrJones::Deprecated::FClockTaskHandle ClockTaskHandle = DEPRECATED_Clock.ScheduleTask(ClockTime, DrJones::Deprecated::FClockTickDelegate::CreateWeakLambda(this, [Event, EventRule](const DrJones::Deprecated::FClockTime& ClockTime)
		{
			if (EventRule->CanEventExecute())
			{
				Event.ExecuteIfBound();
			}
		}));
		ClockTaskHandles.Add(ClockTaskHandle);
	}
	return Handle;
}

void AIlluminati::PostGlobalEvent(const FIlluminatiGlobalEvent& GlobalEvent) const
{
	if (CVarIlluminatiDebug.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Orange, FString::Printf(TEXT("Illuminati Global Event has been posted.")));
	}

	FIlluminatiDelegates::OnGlobalEventReceived.Broadcast(GlobalEvent);
	OnGlobalEventReceived.Broadcast(GlobalEvent);
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

void AIlluminati::FillArchaeologicalSites()
{
	for (auto& SiteData : DefaultSites)
	{
		UArchaeologicalSite* NewSite = NewObject<UArchaeologicalSite>(this);
		NewSite->GeoData = SiteData.GeoData;
		NewSite->LevelName = SiteData.LevelName;
		NewSite->SiteName = SiteData.SiteName;
		ArchaeologicalSites.Emplace(SiteData.SiteName, NewSite);
	}
}
