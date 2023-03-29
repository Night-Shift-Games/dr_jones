// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "Player/DrJonesCharacter.h"
#include "Items/Item.h"


void AArtifact::Interact(APawn* Indicator)
{
	Take(Indicator);
}

void AArtifact::Take(APawn* Indicator)
{
	OnArtifactTake(Indicator);
}



