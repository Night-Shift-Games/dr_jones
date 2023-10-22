// Property of Night Shift Games, all rights reserved.

#include "DigSite.h"

ADigSite::ADigSite()
{
	PrimaryActorTick.bCanEverTick = false;
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMesh"));
	DynamicMeshComponent->SetupAttachment(RootComponent);
}

void ADigSite::BeginPlay()
{
	Super::BeginPlay();
}

void ADigSite::SetupDigSite(const FVector& DigSiteLocation)
{
	// Snap to world grid.
	const FVector NewLocation = FVector(
		FMath::GridSnap(DigSiteLocation.X, 100.f),
		FMath::GridSnap(DigSiteLocation.Y, 100.f),
		DigSiteLocation.Z);
	SetActorLocation(NewLocation);
	
	//DynamicMeshComponent->SetMesh(MeshData);
}



