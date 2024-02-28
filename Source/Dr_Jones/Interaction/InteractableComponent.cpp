// Property of Night Shift Games, all rights reserved.

#include "Interaction/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInteractableComponent::Interact(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	if (InteractDelegate.IsBound())
	{
		InteractDelegate.Broadcast(Player);
		return true;
	}
	return false;
}

bool UInteractableComponent::AltInteract(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	
	if (AltInteractDelegate.IsBound())
	{
		AltInteractDelegate.Broadcast(Player);
		return true;
	}
	return false;
}

UMeshComponent* UInteractableComponent::GetOwnerMesh() const
{
	USceneComponent* ParentSceneComponent = GetAttachParent();
	if (ParentSceneComponent && ParentSceneComponent->GetOwner() != GetOwner())
	{
		ParentSceneComponent = nullptr;
	}
	return ParentSceneComponent ? Cast<UMeshComponent>(ParentSceneComponent) : GetOwner()->FindComponentByClass<UMeshComponent>();
}

void UInteractableComponent::SetRenderPostProcessInteractionOutline(bool bRender) const
{
	if (UMeshComponent* MeshComponent = GetOwnerMesh())
	{
		MeshComponent->SetRenderCustomDepth(true);
		MeshComponent->SetCustomDepthStencilValue(bRender ? 1 : 0);
	}
}
