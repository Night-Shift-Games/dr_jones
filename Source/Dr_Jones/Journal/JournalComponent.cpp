// Property of Night Shift Games, all rights reserved.

#include "JournalComponent.h"

#include "EnhancedInputComponent.h"
#include "UI/WidgetManager.h"
#include "InputAction.h"
#include "UI/DrJonesWidgetBase.h"
#include "Utilities/Utilities.h"

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
	const UWidgetManager& Manager = Utilities::GetWidgetManager(*this);
	Manager.SetWidgetVisibility(JournalUIClass, Manager.GetWidget(JournalUIClass)->IsVisible() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

