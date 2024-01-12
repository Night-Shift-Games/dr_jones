// Property of Night Shift Games, all rights reserved.


#include "ArchaeologistDesk.h"


AArchaeologistDesk::AArchaeologistDesk()
{
	DeskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = DeskMesh;
	PrimaryActorTick.bCanEverTick = false;
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable Component"));
	InteractableComponent->InteractDelegate.AddDynamic(this, &AArchaeologistDesk::OnInteract);
}

void AArchaeologistDesk::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArchaeologistDesk::OnInteract(ADrJonesCharacter* Player)
{
	// It should depend which part of the desk is clicked - Components?
}