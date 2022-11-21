// Property of Night Shift Games, all rights reserved.


#include "Storage.h"

UStorage::UStorage()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FItemStorage UStorage::GetStorage()
{
	return Storage;
}


void UStorage::BeginPlay()
{
	Super::BeginPlay();
	
}

void UStorage::AddItem(UClass* ItemToAdd)
{
	//Storage.Items.Add(ItemToAdd);
}

void UStorage::RemoveItem(UItem* ItemToRemove)
{
}
