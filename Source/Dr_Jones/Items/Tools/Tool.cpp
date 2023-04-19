// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Player/DrJonesCharacter.h"
#include "SharedComponents/ActionComponent.h"
#include "SharedComponents/InteractableComponent.h"

ATool::ATool()
{
	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
}

void ATool::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractDelegate.AddUniqueDynamic(this, &ATool::PickUp);
}

void ATool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InteractableComponent->InteractDelegate.RemoveDynamic(this, &ATool::PickUp);
}

void ATool::PickUp()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10, FColor::Red, "Test");
}
