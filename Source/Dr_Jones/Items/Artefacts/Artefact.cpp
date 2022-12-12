// Property of Night Shift Games, all rights reserved.

#include "Artefact.h"
#include "../../RuntimeCharacter.h"

void UArtefact::BeginPlay()
{
	Super::BeginPlay();
	SetStaticMesh(ItemMesh);
}

void UArtefact::Interact(APawn* Indicator)
{
	Take(Indicator);
}

void UArtefact::Take(APawn* Indicator)
{
	Cast<ARuntimeCharacter>(Indicator)->AddArtefact(this);
	OnArtefactTake(Indicator);
	this->DestroyComponent();
}

FString UArtefact::GetInteractSentence()
{
	return InteractionSentence + ItemName.ToString();
}

void UArtefact::SetParameters(UArtefact* Other)
{
	InteractionSentence = Other->InteractionSentence;
	Rarity = Other->Rarity;
	Size = Other->Size;
	ItemImage = Other->ItemImage;
	ItemName = Other->ItemName;
	ItemMesh = Other->ItemMesh;
	Description = Other->Description;
}

