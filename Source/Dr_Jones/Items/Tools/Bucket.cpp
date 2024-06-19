// Property of Night Shift Games, all rights reserved.

#include "Bucket.h"

#include "Equipment/EquipmentComponent.h"
#include "Utilities/Utilities.h"

void ABucket::OnUnequip()
{
	Super::OnUnequip();
	
	UEquipmentComponent* Equipment = Utilities::GetPlayerCharacter(*this).GetEquipment();
	Equipment->DetachItemFromHand(*this);
}

void ABucket::OnRemovedFromEquipment()
{
	Super::OnRemovedFromEquipment();
	
	const ADrJonesCharacter* Player = GetInstigator<ADrJonesCharacter>();
	const FVector GroundLocation = GetLocationOfItemAfterDropdown();
	const FRotator Rotation = Player ? FRotator(0.0, Player->GetActorRotation().Yaw, 0.0) : FRotator::ZeroRotator; 

	SetActorLocationAndRotation(GroundLocation, Rotation);
}