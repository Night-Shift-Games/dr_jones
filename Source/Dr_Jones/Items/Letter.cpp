// Property of Night Shift Games, all rights reserved.

#include "Items/Letter.h"

ALetter::ALetter()
{
	LetterMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = LetterMeshComponent;
}

void ALetter::OnEquip()
{
	Super::OnEquip();
	LetterMeshComponent->SetVisibility(true);
}

void ALetter::OnUnequip()
{
	Super::OnUnequip();
	LetterMeshComponent->SetVisibility(false);
}
