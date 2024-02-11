// Property of Night Shift Games, all rights reserved.

#include "Player/PlayerComponents/InteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Player/DrJonesCharacter.h"
#include "Player/WidgetManager.h"
#include "SharedComponents/InteractableComponent.h"
#include "Utilities.h"

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

	check(Owner.IsValid());
	
	SelectedInteractiveComponent = FetchInteractiveComponent();

	if (SelectedInteractiveComponent == PreviousSelectedInteractiveActor)
	{
		return;
	}
	
	UpdateInteractionUI();
	
	PreviousSelectedInteractiveActor = SelectedInteractiveComponent;
}

void UInteractionComponent::SetupPlayerInput(UEnhancedInputComponent* InputComponent)
{
	InputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &UInteractionComponent::Interact);
	InputComponent->BindAction(AlternativeInteractionAction, ETriggerEvent::Triggered, this, &UInteractionComponent::AltInteract);
}

UInteractableComponent* UInteractionComponent::FetchInteractiveComponent() const
{
	if (SelectedInteractiveComponent && SelectedInteractiveComponent->IsInteractionInProgress())
	{
		return SelectedInteractiveComponent;
	}
	UInteractableComponent* InteractableComponent = nullptr;
	if (const USceneComponent* FoundSceneComponent = Utilities::GetPlayerLookingAt(InteractionRange).GetComponent())
	{
		InteractableComponent = GetAttachedInteractableComponent(*FoundSceneComponent);
		if (InteractableComponent && InteractableComponent->IsInteractionEnabled())
		{
			return InteractableComponent;
		}
	}
	if (const AActor* Actor = Utilities::GetPlayerLookingAt(InteractionRange).GetActor())
	{
		InteractableComponent = Actor->FindComponentByClass<UInteractableComponent>();
		if (InteractableComponent && InteractableComponent->IsInteractionEnabled() && Actor->GetRootComponent() == InteractableComponent->GetAttachParent())
		{
			return InteractableComponent;
		}
	}
	return InteractableComponent && InteractableComponent->IsInteractionEnabled() ? InteractableComponent : nullptr;
}

void UInteractionComponent::UpdateInteractionUI()
{
	Utilities::GetWidgetManager(*this).SetWidgetVisibility(InteractionUI, SelectedInteractiveComponent ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
	if (PreviousSelectedInteractiveActor && SelectedInteractiveComponent != PreviousSelectedInteractiveActor)
	{
		PreviousSelectedInteractiveActor->SetRenderPostProcessInteractionOutline(false);
	}

	if (SelectedInteractiveComponent)
	{
		SelectedInteractiveComponent->SetRenderPostProcessInteractionOutline(true);
	}
}

void UInteractionComponent::Interact()
{
	if (!SelectedInteractiveComponent || !SelectedInteractiveComponent->IsInteractionEnabled())
	{
		return;
	}
	
	SelectedInteractiveComponent->Interact(Owner.Get());
}

void UInteractionComponent::AltInteract()
{
	if (!SelectedInteractiveComponent || !SelectedInteractiveComponent->IsInteractionEnabled())
	{
		return;
	}
	
	SelectedInteractiveComponent->AltInteract(Owner.Get());
}

/*static*/ bool UInteractionComponent::IsInteractable(const AActor& ActorToCheck)
{
	const UInteractableComponent* InteractableComponent = ActorToCheck.FindComponentByClass<UInteractableComponent>();
	return IsValid(InteractableComponent) && InteractableComponent->IsInteractionEnabled();
}

bool UInteractionComponent::IsInteractable(const UMeshComponent& ComponentToCheck)
{
	return IsValid(GetAttachedInteractableComponent(ComponentToCheck));
}

UInteractableComponent* UInteractionComponent::GetAttachedInteractableComponent(
	const USceneComponent& ComponentToCheck)
{
	TArray<USceneComponent*> Children;
	ComponentToCheck.GetChildrenComponents(false, Children);
	for (const auto Child : Children)
	{
		if (Child->IsA<UInteractableComponent>())
		{
			return Cast<UInteractableComponent> (Child);
		}
	}
	return nullptr;
}


