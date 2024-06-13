// Property of Night Shift Games, all rights reserved.

#include "JournalComponent.h"

#include "EnhancedInputComponent.h"
#include "UI/WidgetManager.h"
#include "InputAction.h"
#include "Components/WidgetComponent.h"
#include "Equipment/EquipmentComponent.h"
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

void UJournalComponent::ConstructJournal()
{
	for (auto& KV : Chapters)
	{
		FJournalChapter Chapter = KV.Value;
		Chapter.NumberOfPages = Chapter.Pages.Num();
	}
}

void UJournalComponent::OpenJournal()
{
	UEquipmentComponent* EquipmentComponent = GetOwner<ADrJonesCharacter>()->GetEquipment();
	NS_EARLY(!EquipmentComponent);
	const AItem* ItemInHand = EquipmentComponent->GetItemInHand();
	if (ItemInHand && ItemInHand->IsA(JournalItem))
	{
		EquipmentComponent->UnequipItem();
	}
	else
	{
		EquipmentComponent->EquipItemByClass(JournalItem);
	}
}

void UJournalComponent::SetActivePage(int NewPage)
{
}
