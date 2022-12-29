// Property of Night Shift Games, all rights reserved.


#include "QuestNote.h"

AQuestNote::AQuestNote()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AQuestNote::Interact(APawn* Indicator)
{
	K2_OnInteract(Indicator);
}

FString AQuestNote::GetInteractSentence()
{
	return TEXT("Pickup Note");
}
