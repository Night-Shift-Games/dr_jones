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
				TSharedPtr<FSubChunk> NewChunk = SubChunks.Add(IntVector, MakeShared<FSubChunk>(*this, FVector(IntVector), SubChunkResolution));
				FHitResult OutHit;
				GWorld->LineTraceSingleByChannel(OutHit,
					NewChunk->GetWorldLocation(),
					NewChunk->GetWorldLocation() - FVector (0,0,SubChunkResolution / 4),
					ECollisionChannel::ECC_Visibility);
				if (OutHit.bBlockingHit)
				{
					NewChunk->bSurface = true;
				}
				else
				{
					GWorld->LineTraceSingleByChannel(OutHit,
					NewChunk->GetWorldLocation(),
					NewChunk->GetWorldLocation() + FVector (0,0,SubChunkResolution / 4),
					ECollisionChannel::ECC_Visibility);
					if (OutHit.bBlockingHit)
					{
						NewChunk->bSurface = true;
					}
				}
				
			}
		}
	}
	CreateSurface();
}

int8 FMasterChunk::FindDirectionSign(float Lenght)
{
	const float SubChunkResolution = Resolution / 3.f;
	const float SubChunkHalfResolution = SubChunkResolution / 2.f;
	// CHECK NORTH / SOUTH
	if (Lenght > SubChunkResolution + SubChunkHalfResolution || Lenght < -SubChunkResolution - SubChunkHalfResolution)
	{
		return 3;
	}
	if (Lenght < -SubChunkHalfResolution && Lenght)
	{
		return -1;
	}
	else if (Lenght > SubChunkHalfResolution && Lenght)
	{
		return 1;
	}
	return 0;
}

FSubChunk* FMasterChunk::GetSubChunkAtLocation(const FVector& Location)
{
	const FVector LocalLocation = FTransform(WorldLocation).InverseTransformPosition(Location);

	FIntVector3 IntVector = FIntVector3::ZeroValue;
	IntVector.X = FindDirectionSign(LocalLocation.X);
	IntVector.Y = FindDirectionSign(LocalLocation.Y);
	IntVector.Z = FindDirectionSign(LocalLocation.Z);
	
	IntVector *= Resolution / 3.f;
	
	if (const TSharedPtr<FSubChunk>* SharedChunk = SubChunks.Find(IntVector))
	{
		return SharedChunk->Get();
	}
	//MakeShared<FSubChunk>(*this, FVector(IntVector), Resolution)
	return nullptr;
}

void FMasterChunk::RefreshMesh()
{
	TArray<FVector> MeshDesc;
	MeshDesc.Reserve(SubChunks.Num());
	for (auto& Chunk : SubChunks)
	{
		// if (Chunk.Value->bSolid && DiffersFromNeighbours(*Chunk.Value))
		// {
		// 	MeshDesc.Add(FVector(Chunk.Key));
		// 	UE::Geometry::FVertexInfo VertexInfo;
		// 	VertexInfo.Position = FVector(Chunk.Value->GetWorldLocation());
		// 	DynamicMeshComponent->GetMesh()->AppendVertex(VertexInfo);
		// }
	}
	
}

bool FMasterChunk::IsSurface(FSubChunk* Chunk)
{
	TArray<FSubChunk*> Chunks;
	Chunks.Add(Chunk->FindNeighbor(FIntVector(0, 0, 1)));		// UP
	Chunks.Add(Chunk->FindNeighbor(FIntVector(0, 0, -1)));		// DOWN
	Chunks.Add(Chunk->FindNeighbor(FIntVector(0, 1, 0)));		// LEFT
	Chunks.Add(Chunk->FindNeighbor(FIntVector(0, -1, 0)));		// RIGHT 
	Chunks.Add(Chunk->FindNeighbor(FIntVector(1, 0, 0)));		// FRONT
	Chunks.Add(Chunk->FindNeighbor(FIntVector(-1, 0, 0)));		// BACK

	for (auto* Neighbor : Chunks)
	{
		if (!Neighbor || !Neighbor->bSolid)
		{
			return true;
		}
	}
	return false;
}

void FMasterChunk::CreateSurface()
{
	// Iterate over all chunks.
	for (auto& KV : SubChunks)
	{
		FSubChunk* Chunk = KV.Value.Get();
		// Check if it is filled or not. If air skip. 	// Get Neighboring chunks. 	// Check if all neighbours are same as this chunk is.
		if (!Chunk->bSolid || !IsSurface(Chunk))
		{
			continue;
		}
		// If there is difference add it to surface net. (As it must see air).
		SurfaceChunks.Add(KV.Key, KV.Value);
		Chunk->bSurface = true;
	}
}

FSubChunk::FSubChunk(FMasterChunk& NewOwner, const FVector& NewLocation, int NewResolution) : FChunk(NewLocation, NewResolution), Owner(NewOwner)
{
	RelativeLocation = NewLocation;
	WorldLocation = Owner.GetWorldLocation() + RelativeLocation;
}

FSubChunk* FSubChunk::FindNeighbor(const FIntVector& NeighborOrientation) const
{
	const FVector NeighborLocation = WorldLocation + FVector(NeighborOrientation) * Resolution;
	return Owner.GetSubChunkAtLocation(NeighborLocation);
}
