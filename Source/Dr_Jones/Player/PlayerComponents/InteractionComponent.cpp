// Property of Night Shift Games, all rights reserved.

#include "Player/PlayerComponents/InteractionComponent.h"

#include "Player/DrJonesCharacter.h"
#include "Player/WidgetManager.h"
#include "SharedComponents/InteractableComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
	WidgetManager = Owner->GetWidgetManager();
}

bool UInteractionComponent::IsInteractable() const
{
	return ActorToInteract->FindComponentByClass<UInteractableComponent>() != nullptr;
}

void UInteractionComponent::UpdateInteractionWidget()
{
	if (!ActorToInteract || !IsInteractable())
	{
		WidgetManager->HideWidget(InteractionUI);
		return;
	}
	if (!WidgetManager->GetWidget(InteractionUI))
	{
		WidgetManager->AddWidget(InteractionUI);
	}
	WidgetManager->ShowWidget(InteractionUI);
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(Owner.IsValid());
	
	ActorToInteract = Owner->GetPlayerLookingAt(InteractionRange).GetActor();

	if (ActorToInteract == PreviousActorToInteract)
	{
		return;
	}
	PreviousActorToInteract = ActorToInteract;
	UpdateInteractionWidget();
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
	Interactable->Interact(Owner.Get());
}

void UInteractionComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAction("Interact", IE_Pressed, this, &UInteractionComponent::Interact);
}


