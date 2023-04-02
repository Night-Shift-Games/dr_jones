// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Player/DrJonesCharacter.h"
#include "SharedComponents/InteractableComponent.h"

ATool::ATool()
{
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void ATool::BindTool(ADrJonesCharacter& Player)
{
	Player.OnPrimaryActionKeyPressed.AddUniqueDynamic(this, &ATool::UseToolPrimaryAction);
}

void ATool::UnbindTool(ADrJonesCharacter& Player)
{
	Player.OnPrimaryActionKeyPressed.RemoveDynamic(this, &ATool::UseToolPrimaryAction);
}

void ATool::PickUp()
{

}
