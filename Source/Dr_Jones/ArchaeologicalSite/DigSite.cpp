// Property of Night Shift Games, all rights reserved.

#include "DigSite.h"

#include "Dr_Jones.h"
#include "Utilities.h"
#include "Voxel/VoxelEngineUObjectInterface.h"

ADigSite::ADigSite()
{
	PrimaryActorTick.bCanEverTick = false;

	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMesh"));
	DynamicMeshComponent->SetupAttachment(RootComponent);

	VoxelGrid = CreateDefaultSubobject<UVoxelGrid>(TEXT("VoxelGrid"));
}

void ADigSite::Dig(const FVector& Location)
{
	SCOPED_NAMED_EVENT(DigSite_Dig, FColorList::PaleGreen)

	static constexpr float DigRadius = 20.0f;

	NSVE::FVoxelGrid& InternalGrid = VoxelGrid->GetInternal();
	NSVE::FVoxelChunk* Chunk = InternalGrid.GetChunkByIndex(InternalGrid.CalcChunkIndexFromWorldPosition(Location));
	if (!Chunk)
	{
		return;
	}

	NSVE::FVoxelChunk::FTransformData TransformData = Chunk->MakeTransformData();
	Chunk->Voxels.Iterate([&TransformData, &Location](NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::GridPositionToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		Voxel.bSolid &= !bIsInRadius;
	});

	UpdateMesh(true);
}

void ADigSite::UpdateMesh(bool bAsync)
{
	UDynamicMesh* DynamicMesh = DynamicMeshComponent->GetDynamicMesh();
	const NSVE::FVoxelGrid& Grid = VoxelGrid->GetInternal();
	int32 VerticesCount;
	int32 TriangleCount;
	auto UpdateMeshComponentFn = [this, bAsync]
	{
		DynamicMeshComponent->bUseAsyncCooking = bAsync;
		DynamicMeshComponent->SetComplexAsSimpleCollisionEnabled(true, false);
		DynamicMeshComponent->UpdateCollision(true);
	};
	UVoxelEngineUtilities::TriangulateVoxelGrid_Internal(Grid, DynamicMesh, VerticesCount, TriangleCount, MoveTemp(UpdateMeshComponentFn), true);
}

void ADigSite::BeginPlay()
{
	Super::BeginPlay();

	UpdateMesh(false);
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



