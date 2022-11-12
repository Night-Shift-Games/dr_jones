// Property of Night Shift Games, all rights reserved.


#include "Storage.h"

UStorage::UStorage()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStorage::BeginPlay()
{
	Super::BeginPlay();

	
}

void UStorage::AddItem(UItem* ItemToAdd)
{
	Storage.Items.Add(ItemToAdd);
}

void UStorage::RemoveItem(UItem* ItemToRemove)
{
}
