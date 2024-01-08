// Property of Night Shift Games, all rights reserved.

#include "ReactionComponent.h"

#include "Items/Item.h"
#include "Player/DrJonesCharacter.h"
#include "SharedComponents/ActionComponent.h"

void UReactionComponent::CallAction()
{
	if (!ActiveItem.Get<0>())
	{
		return;
	}
	ActiveItem.Value->CallPrimaryAction(GetOwner<ADrJonesCharacter>());
}

void UReactionComponent::CallSecondaryAction()
{
	if (!ActiveItem.Get<0>())
	{
		return;
	}
	ActiveItem.Value->CallSecondaryAction(GetOwner<ADrJonesCharacter>());
}

void UReactionComponent::SetActiveItem(AItem* NewActiveItem)
{
	UActionComponent* ActionComponent = NewActiveItem ? NewActiveItem->FindComponentByClass<UActionComponent>() : nullptr;
	ActiveItem = MakeTuple(NewActiveItem, ActionComponent);
}

AItem* UReactionComponent::GetActiveItem() const
{
	return ActiveItem.Key.Get();
}

void UReactionComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAction("PrimaryItemAction", IE_Pressed, this, &UReactionComponent::CallAction);
	InputComponent->BindAction("SecondaryItemAction", IE_Pressed, this, &UReactionComponent::CallSecondaryAction);
}
