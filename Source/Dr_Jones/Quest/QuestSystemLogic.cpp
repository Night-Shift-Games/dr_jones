// Property of Night Shift Games, all rights reserved.


#include "QuestSystemLogic.h"
#include "Quest.h"
#include "QuestData.h"


void UQuestSystemLogic::AddQuest(UQuest* NewQuest)
{
	check(NewQuest);

	// Don't add the same quest twice
	// @TODO: think about it
	if (Quests.Contains(NewQuest->GetQuestData().QuestData.Name))
	{
		return;
	}

	Quests.Emplace(NewQuest->GetQuestData().QuestData.Name, NewQuest);

	NewQuest->CompletedDelegate.BindUObject(this, &UQuestSystemLogic::OnQuestCompleted);

	OnQuestReceived(NewQuest);
}

void UQuestSystemLogic::OnQuestReceived(UQuest* Quest)
{
	check(Quest);

	QuestReceivedDelegate.Broadcast(Quest);
}

void UQuestSystemLogic::OnQuestCompleted(UQuest* Quest)
{
	check(Quest);

	QuestCompletedDelegate.Broadcast(Quest);
}

