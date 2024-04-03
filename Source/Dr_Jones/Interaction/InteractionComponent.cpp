// Property of Night Shift Games, all rights reserved.

#include "Interaction/InteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "InteractionWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "Interaction/InteractableComponent.h"
#include "Player/DrJonesCharacter.h"
#include "UI/WidgetManager.h"
#include "Utilities/InputTriggerPreventAction.h"
#include "Utilities/Utilities.h"

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
	const FHitResult PlayerSightResult = Utilities::GetPlayerSightTarget(InteractionRange, *this);
	
	if (!PlayerSightResult.IsValidBlockingHit())
	{
		return nullptr;
	}
	
	if (const USceneComponent* FoundSceneComponent = PlayerSightResult.GetComponent())
	{
		InteractableComponent = GetAttachedInteractableComponent(*FoundSceneComponent);
		if (InteractableComponent && InteractableComponent->IsInteractionEnabled())
		{
			return InteractableComponent;
		}
	}
	if (const AActor* Actor = PlayerSightResult.GetActor())
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

	if (!SelectedInteractiveComponent)
	{
		return;
	}
	
	SelectedInteractiveComponent->SetRenderPostProcessInteractionOutline(true);
	const bool bHaveAltInteraction = SelectedInteractiveComponent->AltInteractDelegate.IsBound();

	UWidgetManager::RequestUpdateWidget<UInteractionWidgetDataObject>(*this, InteractionUI, [&](UInteractionWidgetDataObject& Data)
	{
		Data.HasAltInteraction = bHaveAltInteraction;
	});
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
	
	if (!SelectedInteractiveComponent->AltInteract(Owner.Get()))
	{
		UInputTriggerPreventAction* PreventAction = nullptr;
		InteractionAction->Triggers.FindItemByClass<UInputTriggerPreventAction>(&PreventAction);
		if (!PreventAction)
		{
			return;	
		}
		PreventAction->bTriggered = false;
	}
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
	static TArray<USceneComponent*>Children;
	Children = ComponentToCheck.GetAttachChildren();
	const AActor* OwnerToCheck = ComponentToCheck.GetOwner();

	for (const auto Child : Children)
	{
		if (Child->IsA<UInteractableComponent>() && Child->GetOwner() == OwnerToCheck)
		{
			return Cast<UInteractableComponent>(Child);
		}
	}
	
	return nullptr;
}


