// Property of Night Shift Games, all rights reserved.


#include "InteractiveActor.h"


AInteractiveActor::AInteractiveActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactive Actor Static Mesh"));
}

void AInteractiveActor::Interact(APawn* Indicator)
{
	OnInteract(Indicator);
}

FString AInteractiveActor::GetInteractSentence()
{
	return InteractionSentence;
}
