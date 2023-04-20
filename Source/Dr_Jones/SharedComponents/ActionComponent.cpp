// Property of Night Shift Games, all rights reserved.

#include "ActionComponent.h"

UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActionComponent::CallPrimaryAction()
{
	PrimaryActionDelegate.Broadcast();
}

void UActionComponent::CallSecondaryAction()
{
	SecondaryActionDelegate.Broadcast();
}


