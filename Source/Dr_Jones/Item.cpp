// Property of Night Shift Games, all rights reserved.


#include "Item.h"

// Sets default values for this component's properties
UItem::UItem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}


// Called when the game starts
void UItem::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMesh)
	{
		StaticMeshComponent->SetStaticMesh(StaticMesh);
	}
	
}


