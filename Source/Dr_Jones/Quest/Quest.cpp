// Property of Night Shift Games, all rights reserved.

#include "Quest/Quest.h"

#include "Dr_Jones.h"

UQuest_OLD* UQuest_OLD::Construct(const UQuestData& QuestAsset)
{
	UQuest_OLD* Quest = NewObject<UQuest_OLD>();
	Quest->QuestData = &QuestAsset;
	return Quest;
}

UQuest_OLD* UQuest_OLD::Construct(const FDataTableRowHandle& QuestRow)
{
	UQuest_OLD* Quest = NewObject<UQuest_OLD>();
	Quest->QuestTableRow = QuestRow;
	return Quest;
}

UQuest_OLD::UQuest_OLD() :
	bIsCompleted(false)
{
}

void UQuest_OLD::OnAdded()
{
	K2_OnAdded();
}

void UQuest_OLD::OnRemoved()
{
	K2_OnRemoved();
}

void UQuest_OLD::OnCompleted()
{
	K2_OnCompleted();
	CompletedDelegate.ExecuteIfBound(this);
}

void UQuest_OLD::MarkAsCompleted()
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
