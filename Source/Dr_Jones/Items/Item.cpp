// Property of Night Shift Games, all rights reserved.

#include "Item.h"

#include "SharedComponents/ActionComponent.h"
#include "SharedComponents/InteractableComponent.h"

AItem::AItem()
{
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
}

void AItem::SetInteractionEnabled(bool bEnabled) const
{
	check(InteractableComponent);
	InteractableComponent->SetInteractionEnabled(bEnabled);
}

void AItem::SetAttachmentPhysics()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	if (UMeshComponent* Mesh = GetMeshComponent())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetSimulatePhysics(false);
	}
}

void AItem::SetGroundPhysics()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	if (UMeshComponent* Mesh = GetMeshComponent())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		Mesh->SetSimulatePhysics(false);
	}
}

void AItem::SetupItemInHandProperties()
{
	SetInteractionEnabled(false);
	SetAttachmentPhysics();
	// By default we should hide mesh in hand if it's not active.
	GetMeshComponent()->SetVisibility(false);
}

void AItem::SetupItemGroundProperties()
{
	SetInteractionEnabled(true);
	SetGroundPhysics();
}
