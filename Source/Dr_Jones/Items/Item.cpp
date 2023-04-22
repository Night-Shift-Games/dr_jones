// Property of Night Shift Games, all rights reserved.

#include "Item.h"

#include "SharedComponents/InteractableComponent.h"

AItem::AItem()
{
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

UMeshComponent* AItem::GetMeshComponent() const
{
	return FindComponentByClass<UMeshComponent>();
}
