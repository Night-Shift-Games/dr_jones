#include "Chunk.h"

FChunk::FChunk(const FVector& NewLocation, int NewResolution) : WorldLocation(NewLocation)
{
	Resolution = NewResolution;
}

FMasterChunk::FMasterChunk(const FVector& NewLocation, int NewResolution) : FChunk(NewLocation, NewResolution)
{
	DynamicMeshComponent.Reset(NewObject<UDynamicMeshComponent>());
	double SubChunkResolution = Resolution / 3;
	const double CenterOffset = SubChunkResolution;
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int z = 0; z < 3; z++)
			{
				FIntVector3 IntVector = FIntVector3(
					FMath::RoundToInt(SubChunkResolution * x) - CenterOffset,
					FMath::RoundToInt(SubChunkResolution * y) - CenterOffset,
					FMath::RoundToInt(SubChunkResolution * z) - CenterOffset);
				SubChunks.Add(IntVector, MakeShared<FSubChunk>(*this, FVector(IntVector), SubChunkResolution));
			}
		}
	}
}

FSubChunk& FMasterChunk::GetSubChunkAtLocation(const FVector& Location)
{
	FVector LocalLocation = FTransform(WorldLocation).InverseTransformPosition(Location);
#define CHUNK_NORTH 1
#define CHUNK_CENTER 0
#define CHUNK_SOUTH -1

#define CHUNK_EAST 1
#define CHUNK_WEST -1

#define CHUNK_TOP 1
#define CHUNK_BOTTOM -1

	FIntVector3 IntVector = FIntVector3::ZeroValue;
	const float SubChunkResolution = Resolution / 3.f;
	const float SubChunkHalfResolution = SubChunkResolution / 2.f;
	// CHECK NORTH / SOUTH
	if (LocalLocation.X < -SubChunkHalfResolution)
	{
		IntVector.X = CHUNK_SOUTH;
	}
	else if (LocalLocation.X > SubChunkHalfResolution)
	{
		IntVector.X = CHUNK_NORTH;
	}
	else
	{
		IntVector.X = CHUNK_CENTER;
	}
	// CHECK WEST / EAST
	if (LocalLocation.Y < -SubChunkHalfResolution)
	{
		IntVector.Y = CHUNK_WEST;
	}
	else if (LocalLocation.Y > SubChunkHalfResolution)
	{
		IntVector.Y = CHUNK_EAST;
	}
	else
	{
		IntVector.Y = CHUNK_CENTER;
	}
	// CHECK BOTTOM / TOP
	if (LocalLocation.Z < -SubChunkHalfResolution)
	{
		IntVector.Z = CHUNK_BOTTOM;
	}
	else if (LocalLocation.Z > SubChunkHalfResolution)
	{
		IntVector.Z = CHUNK_TOP;
	}
	else
	{
		IntVector.Z = CHUNK_CENTER;
	}
	
	IntVector *= SubChunkResolution;
	
	if (const TSharedPtr<FSubChunk>* SharedChunk = SubChunks.Find(IntVector))
	{
		return *SharedChunk->Get();
	}
	
	return *SubChunks.FindOrAdd(IntVector, MakeShared<FSubChunk>(*this, FVector(IntVector), Resolution));
}

FSubChunk::FSubChunk(FMasterChunk& NewOwner, const FVector& NewLocation, int NewResolution) : FChunk(NewLocation, NewResolution), Owner(NewOwner)
{
	RelativeLocation = NewLocation;
	WorldLocation = Owner.GetWorldLocation() + RelativeLocation;
}