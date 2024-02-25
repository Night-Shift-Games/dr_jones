// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Player/DrJonesCharacter.h"

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
	UEquipmentComponent* EquipmentComponent = Player->GetEquipment();
	checkf(EquipmentComponent, TEXT("Inventory is missing!"));

	EquipmentComponent->AddItem(this);
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

void ATool::OnRemovedFromEquipment()
{
	Super::OnRemovedFromEquipment();
	GetMeshComponent()->SetVisibility(true);
}

void ATool::OnEquip()
{
	Super::OnEquip();
	GetMeshComponent()->SetVisibility(true);
}

void ATool::OnUnequip()
{
	Super::OnUnequip();
	GetMeshComponent()->SetVisibility(false);
}
