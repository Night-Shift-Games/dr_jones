// Property of Night Shift Games, all rights reserved.

#include "Item.h"

UItem::UItem()
{
	PrimaryComponentTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	if (ItemMesh) StaticMeshComponent->SetStaticMesh(ItemMesh);
}

UTexture2D* UItem::GetItemImage()
{
	return ItemImage;
}
