// Property of Night Shift Games, all rights reserved.


#include "QuestSystemLogic.h"
#include "QuestData.h"


void UQuestSystemLogic::OnQuestReceived(UQuestData* Quest)
{
	check(Quest);

	QuestReceivedDelegate.Broadcast(Quest);
	K2_OnQuestReceived(Quest);
}

