// Property of Night Shift Games, all rights reserved.

#include "QuestSystem.h"

#include "Dr_Jones.h"

TOptional<FQuestHandle> UQuest::GetHandle() const
{
	return HandleInRegistry;
}

bool UQuest::IsRegistered() const
{
	return HandleInRegistry.IsSet();
}

UQuest* UQuestSystemComponent::AddQuest(const FQuestDescription& QuestDescription)
{
	if (!QuestDescription.QuestClass)
	{
		UE_LOG(LogDrJones, Error, TEXT("Quest Class has not been specified in the Quest Description."));
		return nullptr;
	}

	UQuest* QuestObject = NewObject<UQuest>(this, QuestDescription.QuestClass);
	check(QuestObject);

	QuestObject->HandleInRegistry = RegisterQuest(*QuestObject);

	return QuestObject;
}

FQuestHandle UQuestSystemComponent::RegisterQuest(UQuest& Quest)
{
	if (Quest.IsRegistered())
	{
		UE_LOG(LogDrJones, Warning, TEXT("Cannot register an already registered quest."));
		return FQuestHandle::NewInvalid();
	}

	FQuestHandle QuestHandle = FQuestHandle::New();
	QuestRegistry.Emplace(QuestHandle, &Quest);
	return QuestHandle;
}
