// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

#include "DynamicMesh/MeshNormals.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Utilities.h"

AArchaeologicalSite::AArchaeologicalSite() 
{
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AArchaeologicalSite::TestDig(FVector Direction, FTransform SphereOrigin)
{
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
	SphereOrigin.SetToRelativeTransform(GetTransform());
	
	int32 ViewportX, ViewportY;
	Controller->GetViewportSize(ViewportX, ViewportY);
	const FVector2D ScreenCenter = FVector2D(ViewportX / 2, ViewportY / 2);

	FVector WorldLocation, WorldDirection;
	if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return;
	}
	WorldDirection.Normalize();
	
	const float Radius = 100.f;
	const FVector DigPoint = WorldDirection * -Radius + SphereOrigin.GetLocation();

	DynamicMeshComponent->GetDynamicMesh()->EditMesh([&](FDynamicMesh3& EditMesh)
	{
		for (const int32 VertexID : EditMesh.VertexIndicesItr())
		{
			const FVector3d Pos = EditMesh.GetVertex(VertexID);
			if (Utilities::IsPointInSphere(Pos, SphereOrigin.GetLocation(), Radius))
			{
				const FVector NewPos = CalculateSphereDeform(Pos, SphereOrigin.GetLocation(), Radius, DigPoint);
				EditMesh.SetVertex(VertexID, NewPos, false);
				DynamicMeshComponent->NotifyMeshUpdated();
			}
		}
	});
}

FVector AArchaeologicalSite::CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const
{
	// Calculate Direction Vector between Vertex and selected point on a sphere
	FVector VertexDirection = VertexPosition - DigDirection;
	const double HalfSize = VertexDirection.Length();
	VertexDirection.Normalize();

	// Calculate Direction Vector between Origin and selected point on a sphere
	FVector OriginDirection = SphereOrigin - DigDirection;
	OriginDirection.Normalize();

	const float ChordLenght = Utilities::GetChordLength(SphereRadius, VertexDirection, OriginDirection);
	const float DeformLenght = ChordLenght - HalfSize;

	return VertexPosition + VertexDirection * DeformLenght;
}

UDynamicMesh* AArchaeologicalSite::AllocateDynamicMesh()
{
	return NewObject<UDynamicMesh>(this);
}

void AArchaeologicalSite::SampleChunk(FVector Location)
{
	FMasterChunk& Chunk = GetChunkAtLocation(Location);
	FSubChunk& SubChunk = Chunk.GetSubChunkAtLocation(Location);
	const FColor SampleColor = FColor::MakeRandomColor();
	SubChunk.Color = SampleColor;
}

FMasterChunk& AArchaeologicalSite::GetChunkAtLocation(FVector Location)
{
	constexpr int ChunkSize = 225.f;
	FIntVector3 IntVector = FIntVector3(
		FMath::RoundToInt(Location.X / ChunkSize),
		FMath::RoundToInt(Location.Y / ChunkSize),
		FMath::RoundToInt(Location.Z / ChunkSize));

	if (const TSharedPtr<FMasterChunk>* SharedChunk = Chunks.Find(IntVector))
	{
		return *SharedChunk->Get();
	}
	
	return *Chunks.Emplace(IntVector, MakeShared<FMasterChunk>(FVector(IntVector * ChunkSize), ChunkSize));
}

void AArchaeologicalSite::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawChunkDebug();
}

void AArchaeologicalSite::DrawChunkDebug()
{
	for (const auto& KV : Chunks)
	{
		FMasterChunk& Chunk = *KV.Value.Get();
		DrawDebugBox(
			GetWorld(),
			Chunk.GetWorldLocation(),
			FVector(Chunk.Resolution / 2.f),
			Chunk.Color);
		if (Chunk.SubChunks.IsEmpty())
		{
			continue;
		}
		for (const auto& SubChunk : Chunk.SubChunks)
		{
			DrawDebugBox(
				GetWorld(),
				SubChunk.Value->GetWorldLocation(),
				FVector(SubChunk.Value->Resolution / 2.f),
				SubChunk.Value->Color);
		}
	}
}