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

void UReactionComponent::SetActiveItem(AItem& NewActiveItem)
{
	UActionComponent* ActionComponent = NewActiveItem.FindComponentByClass<UActionComponent>();
	ActiveItem = MakeTuple(&NewActiveItem, ActionComponent);
}

AItem* UReactionComponent::GetActiveItem() const
{
	return ActiveItem.Key.Get();
}

void UReactionComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAction("PrimaryItemAction", IE_Pressed, this, &UReactionComponent::CallAction);
}
