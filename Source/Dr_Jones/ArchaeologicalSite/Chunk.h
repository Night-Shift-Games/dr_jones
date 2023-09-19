#pragma once
#include "ArchaeologicalSite.h"
#include "Components/DynamicMeshComponent.h"

struct FVoxel;

struct FChunk
{
	FChunk() = default;

	FVoxel* GetVoxelAtLocation(const FVector& Location);

	int Size = 300;
	int Resolution = 10;

	FVector WorldLocation;
	TMap<FIntVector3, TSharedPtr<FVoxel>> Voxels;
	TStrongObjectPtr<UDynamicMeshComponent> Mesh;
	TStrongObjectPtr<AArchaeologicalSite> OwningArchaeologicalSite;
};

struct FVoxel
{
	FVoxel(FChunk& NewOwner, const FVector& NewWorldLocation, const FIntVector3& NewInternalLocation);

	FChunk* OwningChunk;
	FVector WorldLocation;
	FIntVector3 InternalLocation;
	bool Solid = true;
	bool Surface = true;
};


