// Property of Night Shift Games, all rights reserved.


#include "ToolComponent.h"

void UToolComponent::BeginPlay()
{
}

void UToolComponent::AddItem(UItem* ItemToAdd)
{
	UTool* ToolToAdd = Cast<UTool>(ItemToAdd);
	if (ToolToAdd) Storage.Items.Add(ToolToAdd);
}
