// Property of Night Shift Games, all rights reserved.


#include "Quest/Quest.h"


UQuest* UQuest::Construct(const UQuestData& QuestAsset)
{
	UQuest* Quest = NewObject<UQuest>();
	Quest->QuestData = &QuestAsset;
	return Quest;
}

UQuest::UQuest()
{
}

void UQuest::OnAdded()
{
	K2_OnAdded();
}

void UQuest::OnRemoved()
{
	K2_OnRemoved();
}

void UQuest::OnCompleted()
{
	K2_OnCompleted();
	CompletedDelegate.ExecuteIfBound(this);
}
