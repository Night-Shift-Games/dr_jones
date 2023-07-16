#pragma once

struct FSubChunk;

struct FChunk
{
	FChunk(const FVector& Location, int NewResolution);

	const FVector& GetWorldLocation() const { return WorldLocation; }
	const FVector& GetRelativeLocation() const { return RelativeLocation; }
	
	double Resolution = 0;
	FColor Color = FColor::Red;

protected:
	FVector WorldLocation;
	FVector RelativeLocation;
};

struct FMasterChunk : FChunk
{
	FMasterChunk(const FVector& NewLocation, int NewResolution);
	FChunk& GetSubChunkAtLocation(const FVector& Location);
	TMap<FIntVector3, TSharedPtr<FSubChunk>> SubChunks;
};

struct FSubChunk : FChunk
{
	FSubChunk(FMasterChunk& NewOwner, const FVector& NewLocation, int NewResolution);
	FMasterChunk& Owner;
};