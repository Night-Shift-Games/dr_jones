// Property of Night Shift Games, all rights reserved.

#include "QuestSystem.h"

#include "Dr_Jones.h"

TOptional<FQuestHandle> UQuest::GetHandle() const
{
	return HandleInRegistry;
}

UWorld* UQuest::GetWorld() const
{
	const UObject* Outer = GetOuter();
	if (!Outer)
	{
		return nullptr;
	}

	if (HasAnyFlags(RF_ClassDefaultObject) || Outer->HasAnyFlags(RF_BeginDestroyed) || Outer->IsUnreachable())
	{
		return nullptr;
	}

	return Outer->GetWorld();
}

void UQuest::InitializePending()
{
	OnInitializePending();
}

bool UQuest::IsCompleted() const
{
	return bCompleted;
}

const FQuestDescription& UQuest::GetDescription() const
{
	return QuestDescription;
}

bool UQuest::IsRegistered() const
{
	return HandleInRegistry.IsSet();
}

void UQuest::MarkAsCompleted()
{
	check(IsRegistered());

	if (bCompleted)
	{
		UE_LOG(LogDrJones, Warning, TEXT("The Quest %s (%i: %s) is already completed."),
			*QuestDescription.DisplayName.ToString(), HandleInRegistry.GetValue().GetKey(), *GetName());
		return;
	}

	bCompleted = true;
	(void)NotifyCompletedDelegate.ExecuteIfBound();

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest \"%s\" (%s) has been marked as complete."),
		*QuestDescription.DisplayName.ToString(), *GetName());
}

void UQuestSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const FQuestHandle Handle : PendingQuests)
	{
		UQuest* Quest = QuestRegistry.FindChecked(Handle);
		checkf(Quest, TEXT("Null quest found during BeginPlay pending quests iteration."));

		InitializePendingQuest(*Quest);
	}
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

	const FQuestHandle QuestHandle = RegisterQuest(*QuestObject);
	QuestObject->HandleInRegistry = QuestHandle;
	QuestObject->QuestDescription = QuestDescription;

	PendingQuests.Add(QuestHandle);
	InitializePendingQuest(*QuestObject);

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest \"%s\" (%s) has been added to the System."),
		*QuestDescription.DisplayName.ToString(), *QuestObject->GetName());

	return QuestObject;
}

FQuestHandle UQuestSystemComponent::RegisterQuest(UQuest& Quest)
{
	check(Quest.GetWorld());

	if (Quest.IsRegistered())
	{
		UE_LOG(LogDrJones, Warning, TEXT("Cannot register an already registered quest."));
		return FQuestHandle::NewInvalid();
	}

	FQuestHandle QuestHandle = FQuestHandle::New();
	QuestRegistry.Emplace(QuestHandle, &Quest);

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest \"%s\" (%s) has been registered."),
		*Quest.QuestDescription.DisplayName.ToString(), *Quest.GetName());

	return QuestHandle;
}

void UQuestSystemComponent::InitializePendingQuest(UQuest& Quest)
{
	checkf(Quest.IsRegistered(), TEXT("Cannot initialize a not registered quest."));
	check(Quest.GetWorld());

	const FQuestHandle Handle = Quest.HandleInRegistry.GetValue();
	Quest.NotifyCompletedDelegate.BindLambda([this, Handle]
	{
		PendingQuests.Remove(Handle);

		if (QuestSystemUtils::IsDebugEnabled())
		{
			const UQuest* FoundQuest = QuestRegistry.FindChecked(Handle);
			check(FoundQuest);
			QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest \"%s\" (%s) has been removed from pending quests array."),
				*FoundQuest->QuestDescription.DisplayName.ToString(), *FoundQuest->GetName());
		}
	});
	Quest.InitializePending();

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Pending Quest \"%s\" (%s) has been initialized."),
		*Quest.QuestDescription.DisplayName.ToString(), *Quest.GetName());
}
