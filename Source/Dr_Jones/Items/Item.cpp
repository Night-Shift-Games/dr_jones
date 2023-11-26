// Property of Night Shift Games, all rights reserved.

#include "Item.h"

#include "SharedComponents/ActionComponent.h"
#include "SharedComponents/InteractableComponent.h"

AItem::AItem()
{
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
}

UMeshComponent* AItem::GetMeshComponent() const
{
	return FindComponentByClass<UMeshComponent>();
}

UTexture2D* AItem::GetItemIcon() const
{
	return ItemIcon;
}

void AItem::EnableInteraction(bool bEnabled)
{
	InteractableComponent->SetInteractionEnabled(bEnabled);
}
