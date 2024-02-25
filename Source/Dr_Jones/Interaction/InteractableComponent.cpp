// Property of Night Shift Games, all rights reserved.

#include "Interaction/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::Interact(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	
	InteractDelegate.Broadcast(Player);
}

void UInteractableComponent::AltInteract(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing"));
	
	AltInteractDelegate.Broadcast(Player);
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
