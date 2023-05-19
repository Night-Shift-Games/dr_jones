#include "Pickaxe.h"

#include "SharedComponents/ActionComponent.h"

void APickaxe::BeginPlay()
{
	Super::BeginPlay();
	check(ActionComponent);
	ActionComponent->PrimaryActionDelegate.AddDynamic(this, &APickaxe::Dig);
}

void APickaxe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ActionComponent->PrimaryActionDelegate.RemoveDynamic(this, &APickaxe::Dig);
}

void APickaxe::Dig()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, TEXT("Pickaxe Dig Test"));
}
