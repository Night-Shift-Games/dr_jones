// Property of Night Shift Games, all rights reserved.

#include "JournalComponent.h"

#include "EnhancedInputComponent.h"
#include "Utilities.h"
#include "Player/WidgetManager.h"

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
	
	const UWidgetManager& Manager = Utilities::GetWidgetManager(*this);
	Manager.SetWidgetVisibility(JournalUIClass, Manager.GetWidget(JournalUIClass)->IsVisible() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

