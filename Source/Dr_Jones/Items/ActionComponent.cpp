// Property of Night Shift Games, all rights reserved.

#include "ActionComponent.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Player/DrJonesCharacter.h"

UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActionComponent::CallPrimaryAction(ADrJonesCharacter* Character)
{
	PrimaryActionDelegate.Broadcast();
	PlayPrimaryActionMontage(Character);
}

void UActionComponent::CallSecondaryAction(ADrJonesCharacter* Character)
{
	SecondaryActionDelegate.Broadcast();
}

void UActionComponent::PlayPrimaryActionMontage(ADrJonesCharacter* Character) const
{
	if (PrimaryActionMontageBehaviorDelegate.ExecuteIfBound(Character))
	{
		return;
	}
	
	if (PrimaryActionMontage)
	{
		Character->CharacterAnimationComponent->PlayMontage(PrimaryActionMontage);
	}
}

void UActionComponent::PlaySecondaryActionMontage(ADrJonesCharacter* Character) const
{
	if (SecondaryActionMontageBehaviorDelegate.ExecuteIfBound(Character))
	{
		return;
	}
	
	if (SecondaryActionMontage)
	{
		Character->CharacterAnimationComponent->PlayMontage(SecondaryActionMontage);
	}
}
