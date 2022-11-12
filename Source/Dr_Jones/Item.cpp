// Property of Night Shift Games, all rights reserved.


#include "Item.h"

// Sets default values for this component's properties
UItem::UItem()
{
	PrimaryComponentTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}


// Called when the game starts
void UItem::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
	}
	
}
