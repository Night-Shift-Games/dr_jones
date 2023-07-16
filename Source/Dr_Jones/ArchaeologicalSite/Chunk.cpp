#include "Chunk.h"

FChunk::FChunk(const FVector& NewLocation, int NewResolution) : WorldLocation(NewLocation)
{
	Resolution = NewResolution;
}

FMasterChunk::FMasterChunk(const FVector& NewLocation, int NewResolution) : FChunk(NewLocation, NewResolution)
{
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

FChunk& FMasterChunk::GetSubChunkAtLocation(const FVector& Location)
{
	FIntVector3 IntVector = FIntVector3(
		FMath::RoundToInt(Location.X / Resolution),
		FMath::RoundToInt(Location.Y / Resolution),
		FMath::RoundToInt(Location.Z / Resolution));


	if (const TSharedPtr<FSubChunk>* SharedChunk = SubChunks.Find(IntVector))
	{
		return *SharedChunk->Get();
	}
	
	return *SubChunks.Emplace(IntVector, MakeShared<FSubChunk>(*this, FVector(IntVector), Resolution));
}

FSubChunk::FSubChunk(FMasterChunk& NewOwner, const FVector& NewLocation, int NewResolution) : FChunk(NewLocation, NewResolution), Owner(NewOwner)
{
	RelativeLocation = NewLocation;
	WorldLocation = RelativeLocation + Owner.GetWorldLocation();
}