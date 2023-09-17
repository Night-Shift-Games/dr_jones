// Property of Night Shift Games, all rights reserved.


#include "QuestSystemLogic.h"
#include "Quest.h"
#include "QuestData.h"


void UQuestSystemLogic::AddQuest(UQuest_OLD* NewQuest)
{
	check(NewQuest);

	const FName& QuestName = NewQuest->GetQuestName();

	// Don't add the same quest twice
	// @TODO: think about it
	if (Quests.Contains(QuestName))
	{
		return;
	}

	Quests.Emplace(QuestName, NewQuest);

	// The system should get notified when the quest gets completed externally.
	NewQuest->CompletedDelegate.BindUObject(this, &UQuestSystemLogic::OnQuestCompleted);

	NewQuest->OnAdded();
	OnQuestReceived(NewQuest);
}

void UQuestSystemLogic::RemoveQuest(const FName& QuestName)
{
	// Ignoring the retval, but behavior is the same.
	RemoveAndRetrieveQuest(QuestName);
}

UQuest_OLD* UQuestSystemLogic::RemoveAndRetrieveQuest(const FName& QuestName)
{
	ensureAlwaysMsgf(!QuestName.IsNone(), TEXT("Don't use 'None' as a quest name."));

	TObjectPtr<UQuest_OLD> Quest = nullptr;
	if (Quests.RemoveAndCopyValue(QuestName, Quest))
	{
		Quest->OnRemoved();
		OnQuestRemoved(Quest);
	}

	return Quest;
}

UQuest_OLD* UQuestSystemLogic::FindQuest(const FName& QuestName)
{
	ensureAlwaysMsgf(!QuestName.IsNone(), TEXT("Don't use 'None' as a quest name."));

	if (TObjectPtr<UQuest_OLD>* FoundQuest = Quests.Find(QuestName))
	{
		return *FoundQuest;
	}
	return nullptr;
}

void UQuestSystemLogic::K2_FindQuest(const FName& QuestName, bool& bOutFound, UQuest_OLD*& OutQuest)
{
	OutQuest = FindQuest(QuestName);
	bOutFound = (bool)OutQuest;
}

void UQuestSystemLogic::OnQuestReceived(UQuest_OLD* Quest)
{
	check(Quest);

	QuestReceivedDelegate.Broadcast(Quest);
}

void UQuestSystemLogic::OnQuestRemoved(UQuest_OLD* Quest)
{
	check(Quest);

	QuestRemovedDelegate.Broadcast(Quest);
}

void UQuestSystemLogic::OnQuestCompleted(UQuest_OLD* Quest)
{
	check(Quest);
	
	QuestCompletedDelegate.Broadcast(Quest);
}

