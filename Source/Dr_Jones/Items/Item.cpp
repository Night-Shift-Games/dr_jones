// Property of Night Shift Games, all rights reserved.

#include "Item.h"

UItem::UItem()
{
	PrimaryComponentTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	if (ItemMesh)
	{
		StaticMeshComponent->SetStaticMesh(ItemMesh);
	}
	StaticMeshComponent->SetupAttachment(this);
}

UTexture2D* UItem::GetItemImage()
{
	return ItemImage;
}
