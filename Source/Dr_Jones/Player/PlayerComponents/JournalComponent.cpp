// Property of Night Shift Games, all rights reserved.

#include "JournalComponent.h"

#include "EnhancedInputComponent.h"

UJournalComponent::UJournalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UJournalComponent::SetupPlayerInputComponent(UEnhancedInputComponent& EnhancedInputComponent)
{
	EnhancedInputComponent.BindAction(OpenJournalAction, ETriggerEvent::Triggered, this, &UJournalComponent::OpenJournal);
}

void UJournalComponent::OpenJournal()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, __func__);
}

