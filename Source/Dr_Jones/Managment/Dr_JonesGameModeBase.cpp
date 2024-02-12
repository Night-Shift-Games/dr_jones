// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dr_JonesGameModeBase.h"

#include "Player/DrJonesCharacter.h"
#include "Quest/QuestSystemLogic.h"
#include "World/Illuminati.h"

ADr_JonesGameModeBase::ADr_JonesGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = ADrJonesCharacter::StaticClass();
	QuestSystem = CreateDefaultSubobject<UQuestSystemLogic>(TEXT("QuestSystemLogic"));

	IlluminatiClass = AIlluminati::StaticClass();
}

void ADr_JonesGameModeBase::InitGameState()
{
	Super::InitGameState();
	Illuminati = CastChecked<AIlluminati>(GetWorld()->SpawnActor(IlluminatiClass));
	OnQuestSystemInitializedDelegate.Broadcast();
	Illuminati->FillArchaeologicalSites();
}

void ADr_JonesGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADr_JonesGameModeBase::ExecutePostQuestSystemLoad(const FQuestSystemInitializedDynamicDelegate& Delegate)
{
	if (QuestSystem)
	{
		Delegate.ExecuteIfBound();
	}
	else if (Delegate.IsBound())
	{
		OnQuestSystemInitializedDelegate.Add(Delegate);
	}
}
