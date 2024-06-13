// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Audio/DefaultSoundBank.h"
#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DrJonesCharacter.h"

void ATool::BeginPlay()
{
	Super::BeginPlay();
	
	InteractableComponent->InteractDelegate.AddUniqueDynamic(this, &ATool::PickUp);
	OwningPlayer = GetOwner<ADrJonesCharacter>();
}

void ATool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InteractableComponent->InteractDelegate.RemoveDynamic(this, &ATool::PickUp);

	Super::EndPlay(EndPlayReason);
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
	GetMeshComponent()->SetVisibility(true, true);
}

void ATool::OnEquip()
{
	Super::OnEquip();
	GetMeshComponent()->SetVisibility(true, true);

	USoundBase* Sound = EquipSound;
	if (!EquipSound)
	{
		if (const UDefaultSoundBank* SoundBank = UNightShiftSettings::LoadDefaultSoundBank())
		{
			Sound = SoundBank->EquipSound;
		}
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}

void ATool::OnUnequip()
{
	Super::OnUnequip();
	GetMeshComponent()->SetVisibility(false, true);
}
