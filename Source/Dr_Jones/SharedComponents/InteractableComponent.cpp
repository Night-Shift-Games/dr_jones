// Property of Night Shift Games, all rights reserved.

#include "SharedComponents/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableComponent::Interact(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	
	InteractDelegate.Broadcast(Player);
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

