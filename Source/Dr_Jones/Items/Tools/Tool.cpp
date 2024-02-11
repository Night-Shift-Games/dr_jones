// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Player/DrJonesCharacter.h"
#include "Player/PlayerComponents/EquipmentComponent.h"
#include "SharedComponents/InteractableComponent.h"

void ATool::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractDelegate.AddUniqueDynamic(this, &ATool::PickUp);
	OwningPlayer = GetOwner<ADrJonesCharacter>();
}

void ATool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InteractableComponent->InteractDelegate.RemoveDynamic(this, &ATool::PickUp);
}

void ATool::PickUp(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing!"));
	OwningPlayer = Player;
	UEquipmentComponent* InventoryComponent = Player->GetInventory();
	checkf(InventoryComponent, TEXT("Inventory is missing!"));

	InventoryComponent->AddTool(*this);
}

UAnimMontage* ATool::FindActionMontage(const FName& MontageName) const
{
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot play tool montage because the tool is not owned by any player."));
		return nullptr;
	}

	checkf(OwningPlayer->CharacterAnimationComponent, TEXT("Animation Component is null"));
	return OwningPlayer->CharacterAnimationComponent->FindItemActionMontage(*this, MontageName);
}
