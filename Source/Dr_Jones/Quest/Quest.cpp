// Property of Night Shift Games, all rights reserved.


#include "Quest/Quest.h"
#include "Dr_Jones.h"


UQuest* UQuest::Construct(const UQuestData& QuestAsset)
{
	UQuest* Quest = NewObject<UQuest>();
	Quest->QuestData = &QuestAsset;
	return Quest;
}

UQuest* UQuest::Construct(const FDataTableRowHandle& QuestRow)
{
	UQuest* Quest = NewObject<UQuest>();
	Quest->QuestTableRow = QuestRow;
	return Quest;
}

UQuest::UQuest() :
	bIsCompleted(false)
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

void UQuest::MarkAsCompleted()
{
	// Notify if calling the 
	if (bIsCompleted)
	{
		UE_LOG(LogDrJones, Warning, TEXT("Quest '%s' is already marked as completed."), *GetQuestName().ToString());
		return;
	}

	bIsCompleted = true;
	OnCompleted();
}
