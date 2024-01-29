// Property of Night Shift Games, all rights reserved.

#include "CampEntity.h"
#include "SharedComponents/InteractableComponent.h"

ACampEntity::ACampEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("MainInteractableComponent"));
}

