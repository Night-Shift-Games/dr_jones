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

void ADigSite::Dig(const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_Dig, FColorList::PaleGreen)

	NSVE::FVoxelGrid& InternalGrid = VoxelGrid->GetInternal();

	TArray<NSVE::FVoxelChunk*> ChunksInRadius;
	ChunksInRadius.Reserve(InternalGrid.GetChunkCount());
	const double DigRadiusSquared = DigRadius * DigRadius;
	InternalGrid.IterateChunks([&](NSVE::FVoxelChunk& Chunk, int32 Index)
	{
		if (FMath::SphereAABBIntersection(Location, DigRadiusSquared, Chunk.Bounds.GetBox()))
		{
			ChunksInRadius.Add(&Chunk);
		}
	});
	const int32 ChunkCount = ChunksInRadius.Num();
	ParallelForTemplate(ChunkCount, [ChunksInRadius = MoveTemp(ChunksInRadius), Location, DigRadius](int32 Index)
	{
		DigVoxelsInRadius(*ChunksInRadius[Index], Location, DigRadius);
	});

	UpdateMesh(true);
}

void ADigSite::UnDig(const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_Dig, FColorList::PaleGreen)

	NSVE::FVoxelGrid& InternalGrid = VoxelGrid->GetInternal();

	TArray<NSVE::FVoxelChunk*> ChunksInRadius;
	ChunksInRadius.Reserve(InternalGrid.GetChunkCount());
	const double DigRadiusSquared = DigRadius * DigRadius;
	InternalGrid.IterateChunks([&](NSVE::FVoxelChunk& Chunk, int32 Index)
	{
		if (FMath::SphereAABBIntersection(Location, DigRadiusSquared, Chunk.Bounds.GetBox()))
		{
			ChunksInRadius.Add(&Chunk);
		}
	});
	const int32 ChunkCount = ChunksInRadius.Num();
	ParallelForTemplate(ChunkCount, [ChunksInRadius = MoveTemp(ChunksInRadius), Location, DigRadius](int32 Index)
	{
		UnDigVoxelsInRadius(*ChunksInRadius[Index], Location, DigRadius);
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

	DynamicMeshComponent->SetWorldTransform(FTransform{});
	UpdateMesh(false);

	TArray<UMaterialInterface*> MaterialSet;
	for (auto It = VoxelGrid->GeneratedLayers.CreateConstIterator(); It; ++It)
	{
		if (It.GetIndex() >= 7)
		{
			break;
		}
		MaterialSet.Add(It->Material);
	}
	MaterialSet.SetNumZeroed(8);
	MaterialSet.Insert(VoxelGrid->ArtifactHintMaterial, 7);
	DynamicMeshComponent->ConfigureMaterialSet(MaterialSet);
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

void ADigSite::DigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius)
{
	NSVE::FVoxelChunk::FTransformData TransformData = Chunk.MakeTransformData();
	Chunk.Voxels.Iterate([&TransformData, &Location, DigRadius](NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::GridPositionToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		Voxel.bSolid &= !bIsInRadius;
	});
}

void ADigSite::UnDigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius)
{
	NSVE::FVoxelChunk::FTransformData TransformData = Chunk.MakeTransformData();
	Chunk.Voxels.Iterate([&TransformData, &Location, DigRadius](NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::GridPositionToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		Voxel.bSolid |= bIsInRadius;
	});
}



