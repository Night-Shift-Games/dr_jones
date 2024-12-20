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

void AItem::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractionText = FText::FromString("Pickup");
}

void AItem::SetInteractionEnabled(bool bEnabled) const
{
	check(InteractableComponent);
	InteractableComponent->SetInteractionEnabled(bEnabled);
}

void AItem::SetWorldPhysics()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	TInlineComponentArray<UMeshComponent*> Components;
	GetComponents<UMeshComponent>(Components);
	for (UMeshComponent* Mesh : Components)
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Mesh->SetSimulatePhysics(false);
	}
}

FVector AItem::GetLocationOfItemAfterDropdown() const
{
	TArray<AActor*> ActorArray;
	GetAttachedActors(ActorArray);
	GetInstigator()->GetAttachedActors(ActorArray, false);
	TArray<const AActor*> ConstActorArray;
	ConstActorArray.Add(GetInstigator());
	ConstActorArray.Add(this);
	ConstActorArray.Append(ActorArray);
	FVector GroundLocation = Utilities::FindGround(*this, GetActorLocation(), ConstActorArray).Location;
	GroundLocation.Z -= Utilities::GetMeshZOffset(*this);
	return GroundLocation;
}

void AItem::OnAddedToEquipment()
{
	SetInteractionEnabled(false);
}

void AItem::OnRemovedFromEquipment()
{
	SetInteractionEnabled(true);
}