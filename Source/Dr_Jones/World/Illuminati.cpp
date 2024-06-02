// Property of Night Shift Games, all rights reserved.

#include "Illuminati.h"

#include "Managment/Dr_JonesGameModeBase.h"
#include "Quest/QuestSystem.h"
#include "Utilities/NightShiftMacros.h"

AIlluminati::AIlluminati()
{
	QuestSystemComponent = CreateDefaultSubobject<UQuestSystemComponent>(TEXT("QuestSystem"));
}

void AIlluminati::BeginPlay()
{
	Super::BeginPlay();

	Clock = GetGameInstance()->GetSubsystem<UClock>();
	Clock->StartClock();
}

void AIlluminati::PostGlobalEvent(const FIlluminatiGlobalEvent& GlobalEvent) const
{
#if ENABLE_DRAW_DEBUG
	if (CVarIlluminatiDebug.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Orange, FString::Printf(TEXT("Illuminati Global Event has been posted.")));
	}
#endif
	FIlluminatiDelegates::OnGlobalEventReceived.Broadcast(GlobalEvent);
	OnGlobalEventReceived.Broadcast(GlobalEvent);
}

AIlluminati* AIlluminati::GetIlluminatiInstance(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	NS_EARLY_R(!World, nullptr);

	const ADr_JonesGameModeBase* GameMode = World->GetAuthGameMode<ADr_JonesGameModeBase>();
	NS_EARLY_R(!GameMode, nullptr);

	return GameMode->GetIlluminati();
}

UQuestSystemComponent* AIlluminati::GetQuestSystemInstance(const UObject* WorldContextObject)
{
	const AIlluminati* Illuminati = GetIlluminatiInstance(WorldContextObject);
	NS_EARLY_R(!Illuminati, nullptr);

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
