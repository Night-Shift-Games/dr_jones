#include "Chunk.h"

FVoxel* FChunk::GetVoxelAtLocation(const FVector& Location)
{
	// Transform Location to Internal Location.
	FIntVector3 InternalLocation = FIntVector3(WorldLocation - Location);
	// Clamp Internal Location to fixed voxels positions.
	const int VoxelSize = Size / Resolution;
	InternalLocation = InternalLocation / VoxelSize;
	if (const TSharedPtr<FVoxel>* FoundVoxel = Voxels.Find(InternalLocation))
	{
		return FoundVoxel->Get();
	}
	return nullptr;
}
FVoxel::FVoxel(FChunk& NewOwner, const FVector& NewWorldLocation, const FIntVector3& NewInternalLocation)
{
	OwningChunk = &NewOwner;
	this->WorldLocation = NewWorldLocation;
	this->InternalLocation = NewInternalLocation;
}
