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

	OnQuestCompleted.Broadcast();

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest \"%s\" (%s) has been marked as complete."),
		*QuestDescription.DisplayName.ToString(), *GetName());
}

const FQuestDescription* UQuestChain::GetNextQuest() const
{
	++InternalQuestIndex;
	if (InternalQuestIndex >= Quests.Num())
	{
		return nullptr;
	}

	return &Quests[InternalQuestIndex];
}

void UQuestChain::SetupNextQuest()
{
	const FQuestDescription* QuestDescription = GetNextQuest();
	if (!QuestDescription)
	{
		return;
	}

	UQuest* Quest = OwningQuestSystem->AddQuest(*QuestDescription);
	if (!Quest)
	{
		return;
	}

	Quest->OnQuestCompleted.AddDynamic(this, &UQuestChain::OnQuestCompleted);
}

void UQuestChain::InitializeChain(UQuestSystemComponent* QuestSystemComponent)
{
	checkf(!OwningQuestSystem, TEXT("Quest Chain has already been initialized."));
	OwningQuestSystem = QuestSystemComponent;

	SetupNextQuest();
}

void UQuestChain::OnQuestCompleted()
{
	SetupNextQuest();
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

void UQuestSystemComponent::AddQuestChainByObject(UQuestChain* QuestChain)
{
	if (!QuestChain->HasAnyQuest())
	{
		QuestSystemUtils::LogDebug(TEXT("Tried to add an empty Quest Chain %s"), *QuestChain->GetName());
		return;
	}

	PendingQuestChains.AddUnique(QuestChain);
	QuestChain->InitializeChain(this);
	QuestChain->NotifyCompletedDelegate.BindLambda([this, QuestChain]
	{
		PendingQuestChains.Remove(QuestChain);

		if (QuestSystemUtils::IsDebugEnabled())
		{
			QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest Chain \"%s\" has been removed from pending quest chains array."),
				*QuestChain->GetName());
		}
	});
}

void UQuestSystemComponent::AddQuestChain(const TSubclassOf<UQuestChain>& QuestChainClass)
{
	if (!QuestChainClass)
	{
		UE_LOG(LogDrJones, Error, TEXT("Cannot add quest chain of a null class."));
		return;
	}

	AddQuestChainByObject(NewObject<UQuestChain>(QuestChainClass));
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
