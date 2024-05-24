// Property of Night Shift Games, all rights reserved.

#include "Illuminati.h"

#include "Managment/Dr_JonesGameModeBase.h"
#include "Quest/QuestSystem.h"

AIlluminati::AIlluminati()
{
	QuestSystemComponent = CreateDefaultSubobject<UQuestSystemComponent>(TEXT("QuestSystem"));
	Clock = CreateDefaultSubobject<UClock>(TEXT("WorldClock"));
}

void AIlluminati::BeginPlay()
{
	Super::BeginPlay();

	Clock->InitializeClock(InitialTime);
}

void AIlluminati::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
