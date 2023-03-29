// Property of Night Shift Games, all rights reserved.

#include "Player/PlayerComponents/InteractionComponent.h"

#include "Player/DrJonesCharacter.h"
#include "SharedComponents/InteractableComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Owner.IsValid())
	{
		ActorToInteract = nullptr;
		return;
	}
	ActorToInteract = Owner->GetPlayerLookingAt(InteractionRange).GetActor();

	if (!ActorToInteract)
	{
		Owner->HideInteractionUI();
		return;
	}
	if (ActorToInteract->FindComponentByClass<UInteractableComponent>())
	{
		Owner->ShowInteractionUI();
	}
}

void UInteractionComponent::Interact()
{
	if (!ActorToInteract)
	{
		return;
	}
	UInteractableComponent* Interactable = ActorToInteract->FindComponentByClass<UInteractableComponent>();
	if (!Interactable)
	{
		return;
	}
	Interactable->Interact();
}


