// Property of Night Shift Games, all rights reserved.

#include "Bucket.h"

void ABucket::OnRemovedFromEquipment()
{
	Super::OnRemovedFromEquipment();
	
	const FVector GroundLocation = GetLocationOfItemAfterDropdown();

	const ADrJonesCharacter* Player = GetInstigator<ADrJonesCharacter>();
	const FRotator Rotation = Player ? FRotator(0.0, Player->GetActorRotation().Yaw, 0.0) : FRotator::ZeroRotator; 

	SetActorLocationAndRotation(GroundLocation, Rotation);
}
