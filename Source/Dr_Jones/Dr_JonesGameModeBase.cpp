// Copyright Epic Games, Inc. All Rights Reserved.


#include "Dr_JonesGameModeBase.h"
#include "Quest/QuestSystemLogic.h"

ADr_JonesGameModeBase::ADr_JonesGameModeBase()
{
	DefaultPawnClass = ARuntimeCharacter::StaticClass();
}

void ADr_JonesGameModeBase::BeginPlay()
{
	QuestSystem = NewObject<UQuestSystemLogic>();
}
