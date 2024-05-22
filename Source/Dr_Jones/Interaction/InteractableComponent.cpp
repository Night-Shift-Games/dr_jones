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
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}
	USceneComponent* ParentSceneComponent = GetAttachParent();
	if (ParentSceneComponent && ParentSceneComponent->GetOwner() != Owner)
	{
		ParentSceneComponent = nullptr;
	}
	// TODO: Interactable component shouldn't know anything about artifacts, it creates unneeded dependency - it is a hotfix for double mesh in artifacts.
	return ParentSceneComponent ? Cast<UMeshComponent>(ParentSceneComponent) : Owner->IsA<AArtifact>() ? Cast<AArtifact>(Owner)->GetMeshComponent() : Owner->FindComponentByClass<UMeshComponent>();
}

void UInteractableComponent::SetRenderPostProcessInteractionOutline(bool bRender) const
{
	if (UMeshComponent* MeshComponent = GetOwnerMesh())
	{
		MeshComponent->SetRenderCustomDepth(true);
		MeshComponent->SetCustomDepthStencilValue(bRender ? 1 : 0);
	}
}
