// Property of Night Shift Games, all rights reserved.

#include "Item.h"

#include "Interaction/InteractableComponent.h"
#include "Items/ActionComponent.h"
#include "Utilities/Utilities.h"

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

void AItem::OnAddedToEquipment()
{
	SetInteractionEnabled(false);
	SetHandAttachmentPhysics();
}

void AItem::OnRemovedFromEquipment()
{
	SetInteractionEnabled(true);
	SetWorldPhysics();
}

void AItem::SetHandAttachmentPhysics()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	if (UMeshComponent* Mesh = GetMeshComponent())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		Mesh->SetSimulatePhysics(false);
	}
}

void AItem::SetWorldPhysics()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	if (UMeshComponent* Mesh = GetMeshComponent())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Mesh->SetSimulatePhysics(false);
	}
}

FVector AItem::GetLocationOfItemAfterDropdown() const
{
	FVector GroundLocation = Utilities::FindGround(*this, GetActorLocation(), { this }, ECC_WorldStatic);
	GroundLocation.Z -= Utilities::GetMeshZOffset(*this);
	return GroundLocation;
}
