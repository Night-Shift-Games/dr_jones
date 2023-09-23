// Property of Night Shift Games, all rights reserved.

#include "SharedComponents/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::Interact(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	
	InteractDelegate.Broadcast(Player);
}