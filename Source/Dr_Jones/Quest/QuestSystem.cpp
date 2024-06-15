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

void UQuest::SendQuestMessage(const TScriptInterface<IQuestMessageInterface>& QuestMessage)
{
	OnQuestMessageReceived(QuestMessage);
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

bool UQuestChain::HasNextQuest() const
{
	return InternalQuestIndex < Quests.Num() - 1;
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
	check(InternalQuestIndex == TNumericLimits<int32>::Max());
	checkf(!OwningQuestSystem, TEXT("Quest Chain has already been initialized."));
	checkf(HasAnyQuest(), TEXT("Quest Chain has no quests."));

	OwningQuestSystem = QuestSystemComponent;

	InternalQuestIndex = -1;
	SetupNextQuest();
}

void UQuestChain::OnQuestCompleted()
{
	if (HasNextQuest())
	{
		SetupNextQuest();
		return;
	}

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest Chain %s has been completed."), *GetName());
	(void)NotifyCompletedDelegate.ExecuteIfBound();
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
		QuestSystemUtils::LogDebugIfEnabled(TEXT("Tried to add an empty Quest Chain %s"), *QuestChain->GetName());
		return;
	}

	PendingQuestChains.AddUnique(QuestChain);
	QuestChain->InitializeChain(this);
	QuestChain->NotifyCompletedDelegate.BindLambda([this, QuestChain]
	{
		PendingQuestChains.Remove(QuestChain);

		QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest Chain \"%s\" has been removed from pending quest chains array."),
			*QuestChain->GetName());

		OnQuestChainCompleted.Broadcast(QuestChain);
	});
}

void UQuestSystemComponent::AddQuestChain(const TSubclassOf<UQuestChain>& QuestChainClass)
{
	if (!QuestChainClass)
	{
		UE_LOG(LogDrJones, Error, TEXT("Cannot add quest chain of a null class."));
		return;
	}

	AddQuestChainByObject(NewObject<UQuestChain>(this, QuestChainClass));
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

	OnGlobalQuestAdded.Broadcast(QuestObject);

	return QuestObject;
}

void UQuestSystemComponent::SendQuestMessage(const TScriptInterface<IQuestMessageInterface>& QuestMessage)
{
	TArray<UQuest*> FoundQuests;
	FoundQuests.Reserve(PendingQuests.Num());

	for (const FQuestHandle& QuestHandle : PendingQuests)
	{
		if (UQuest* Quest = FindQuest(QuestHandle))
		{
			FoundQuests.Add(Quest);
		}
	}

	for (UQuest* Quest : FoundQuests)
	{
		Quest->SendQuestMessage(QuestMessage);
	}
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

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Quest %s has been registered."), *Quest.GetName());

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

		UQuest* FoundQuest = QuestRegistry.FindChecked(Handle);
		check(FoundQuest);

		if (QuestSystemUtils::IsDebugEnabled())
		{
			QuestSystemUtils::LogDebug(TEXT("Quest \"%s\" (%s) has been removed from pending quests array."),
				*FoundQuest->QuestDescription.DisplayName.ToString(), *FoundQuest->GetName());
		}

		OnGlobalQuestCompleted.Broadcast(FoundQuest);
	});
	Quest.InitializePending();

	QuestSystemUtils::LogDebugIfEnabled(TEXT("Pending Quest \"%s\" (%s) has been initialized."),
		*Quest.QuestDescription.DisplayName.ToString(), *Quest.GetName());
}

UQuest* UQuestSystemComponent::FindQuest(const FQuestHandle& Handle) const
{
	UQuest* const* FoundQuest = QuestRegistry.Find(Handle);
	if (!FoundQuest)
	{
		return nullptr;
	}

	return *FoundQuest;
}
