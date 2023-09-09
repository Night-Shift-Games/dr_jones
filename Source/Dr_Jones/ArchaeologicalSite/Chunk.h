#pragma once
#include "Components/DynamicMeshComponent.h"

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

struct FMasterChunk : public FChunk
{
	FMasterChunk(const FVector& NewLocation, int NewResolution);
	FSubChunk* GetSubChunkAtLocation(const FVector& Location);
	void RefreshMesh();
	bool IsSurface(FSubChunk* Chunk);

	void CreateSurface();
	
protected:
	int8 FindDirectionSign(float Lenght);
	
public:
	TMap<FIntVector3, TSharedPtr<FSubChunk>> SubChunks;
	TMap<FIntVector3, TSharedPtr<FSubChunk>> SurfaceChunks;
	
protected:
	TStrongObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
};

struct FSubChunk : public FChunk
{
	FSubChunk(FMasterChunk& NewOwner, const FVector& NewLocation, int NewResolution);
	FMasterChunk& Owner;

	FSubChunk* FindNeighbor(const FIntVector& NeighborOrientation) const;
	
	bool bSolid = true;
	bool bSurface = false;
};