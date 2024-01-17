// Property of Night Shift Games, all rights reserved.

#include "Player/PlayerComponents/InteractionComponent.h"

#include "Components/SlateWrapperTypes.h"
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

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(Owner.IsValid());
	
	FindActorToInteract();

	if (ActorToInteract == PreviousActorToInteract)
	{
		return;
	}
	PreviousActorToInteract = ActorToInteract;
	UpdateInteractionWidget();
}

void UInteractionComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAction("Interact", IE_Pressed, this, &UInteractionComponent::Interact);
}

void UInteractionComponent::FindActorToInteract()
{
	AActor* Candidate = Owner->GetPlayerLookingAt(InteractionRange).GetActor();
	ActorToInteract = Candidate && IsInteractable(*Candidate) ? Candidate : nullptr;
}

void UInteractionComponent::UpdateInteractionWidget()
{
	if (!ActorToInteract || !IsInteractable(*ActorToInteract))
	{
		WidgetManager->SetWidgetVisibility(InteractionUI, ESlateVisibility::Hidden);
		return;
	}
	if (!WidgetManager->GetWidget(InteractionUI))
	{
		WidgetManager->AddWidget(InteractionUI);
	}
	WidgetManager->SetWidgetVisibility(InteractionUI, ESlateVisibility::Visible);
}

void UInteractionComponent::Interact()
{
	if (!ActorToInteract)
	{
		return;
	}

	UInteractableComponent* Interactable = ActorToInteract->FindComponentByClass<UInteractableComponent>();
	if (!Interactable || !Interactable->IsInteractionEnabled())
	{
		return;
	}
	Interactable->Interact(Owner.Get());
}

/*static*/ bool UInteractionComponent::IsInteractable(const AActor& ActorToCheck)
{
	const UInteractableComponent* InteractableComponent = ActorToCheck.FindComponentByClass<UInteractableComponent>();
	return IsValid(InteractableComponent) && InteractableComponent->IsInteractionEnabled();
}
