// Property of Night Shift Games, all rights reserved.

#include "DigSite.h"

#include "DigSiteBorder.h"
#include "Engine/OverlapResult.h"
#include "Items/Artifacts/Artifact.h"
#include "Items/Artifacts/ArtifactFactory.h"
#include "Kismet/KismetMathLibrary.h"
#include "Managment/Dr_JonesGameModeBase.h"
#include "Utilities/Utilities.h"
#include "Voxel/VoxelEngineUObjectInterface.h"

ADigSite::ADigSite()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMesh"));
	DynamicMeshComponent->SetupAttachment(RootComponent);
	
	VoxelGrid = CreateDefaultSubobject<UVoxelGrid>(TEXT("VoxelGrid"));
}

void ADigSite::Dig(const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_Dig, FColorList::PaleGreen)

	TArray<NSVE::FVoxelChunk*> ChunksInRadius;
	GetChunksInRadius(Location, DigRadius, ChunksInRadius);

	const int32 ChunkCount = ChunksInRadius.Num();
	ParallelForTemplate(ChunkCount, [this, ChunksInRadius = MoveTemp(ChunksInRadius), Location, DigRadius](int32 Index)
	{
		DigVoxelsInRadius(*ChunksInRadius[Index], Location, DigRadius);
	});

	UpdateMesh(true);
}

FDigSiteSample ADigSite::SampleDig(const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_SampleDig, FColorList::YellowGreen)

	TArray<NSVE::FVoxelChunk*> ChunksInRadius;
	GetChunksInRadius(Location, DigRadius, ChunksInRadius);

	FDigSiteSample DigSiteSample;
	for (const NSVE::FVoxelChunk* Chunk : ChunksInRadius)
	{
		check(Chunk);
		SampleVoxelsInRadius(*Chunk, Location, DigRadius, DigSiteSample.Voxels, DigSiteSample.Artifacts);
	}

	return DigSiteSample;
}

void ADigSite::UnDig(const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_UnDig, FColorList::VioletRed)

	TArray<NSVE::FVoxelChunk*> ChunksInRadius;
	GetChunksInRadius(Location, DigRadius, ChunksInRadius);

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
	UVoxelEngineUtilities::TriangulateVoxelGrid_Internal(Grid, DynamicMesh, VerticesCount, TriangleCount, MoveTemp(UpdateMeshComponentFn), true
#if WITH_EDITORONLY_DATA
		, VoxelGrid->GridVisualizer ? VoxelGrid->GridVisualizer->SurfaceNetsDebugContext : nullptr
#endif
	);
}

void ADigSite::GetChunksInRadius(const FVector& Location, float Radius, TArray<NSVE::FVoxelChunk*>& OutChunks) const
{
	NSVE::FVoxelGrid& InternalGrid = VoxelGrid->GetInternal();

	OutChunks.Reserve(InternalGrid.GetChunkCount());
	const double DigRadiusSquared = Radius * Radius;
	InternalGrid.IterateChunks([&](NSVE::FVoxelChunk& Chunk, int32 Index)
	{
		if (FMath::SphereAABBIntersection(Location, DigRadiusSquared, Chunk.Bounds.GetBox()))
		{
			OutChunks.Add(&Chunk);
		}
	});
}

void ADigSite::BeginPlay()
{
	Super::BeginPlay();

	SpawnArtifacts();
	InitializeVoxelGrid();

	DynamicMeshComponent->SetWorldTransform(FTransform{});
	UpdateMesh(false);

	TArray<UMaterialInterface*> MaterialSet;
	if (MeshMaterial)
	{
		MaterialSet.Add(MeshMaterial);
	}
	DynamicMeshComponent->ConfigureMaterialSet(MaterialSet);

	if (BorderGeneratorClass)
	{
		if (!ensure(BorderGenerator == nullptr))
		{
			BorderGenerator->DestroyComponent();
			BorderGenerator = nullptr;
		}

		FDigSiteRectangularBorderDesc BorderDesc;
		BorderDesc.Center_WS = GetActorLocation();
		BorderDesc.Extents2D = FVector2D{ Extents.X, Extents.Y };
		BorderDesc.Height = Extents.Z;

		BorderGenerator = NewObject<UDigSiteBorder>(this, BorderGeneratorClass);
		BorderGenerator->GenerateRectangularBorder(BorderDesc);
	}
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
	SCOPED_NAMED_EVENT(DigSite_DigVoxelsInRadius, FColorList::CadetBlue)

	NSVE::FVoxelChunk::FTransformData TransformData = Chunk.MakeTransformData();
	TArray<FVector> PointsInRadius;
	Chunk.Voxels.Iterate([&TransformData, &Location, DigRadius, &PointsInRadius](NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::CoordsToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		Voxel.bSolid &= !bIsInRadius;
	});
}

void ADigSite::SampleVoxelsInRadius(const NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius, TArray<FDigSiteVoxelData>& OutVoxels, TArray<AArtifact*>& OutHitArtifacts) const
{
	SCOPED_NAMED_EVENT(DigSite_SampleVoxelsInRadius, FColorList::CadetBlue)

	NSVE::FVoxelChunk::FTransformData TransformData = Chunk.MakeTransformData();
	TArray<FVector> PointsInRadius;
	Chunk.Voxels.Iterate([&TransformData, &Location, DigRadius, &PointsInRadius, &OutVoxels](const NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::CoordsToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		if (bIsInRadius && Voxel.bSolid)
		{
			PointsInRadius.Add(WorldPosition);
			FDigSiteVoxelData& VoxelData = OutVoxels.Emplace_GetRef();
			VoxelData.MaterialIndex = Voxel.LocalMaterial;
		}
	});

	const UWorld* World = GetWorld();
	TSet<AArtifact*> HitArtifacts;
	TArray<FOverlapResult> OverlapResults;
	for (const FVector PointInRadius : PointsInRadius)
	{
		OverlapResults.Reset();
		FCollisionObjectQueryParams ObjectQueryParams;
		// QueryParams.AddObjectTypesToQuery(ECC_);
		if (World->OverlapMultiByObjectType(
			OverlapResults,
			PointInRadius,
			FQuat::Identity,
			ObjectQueryParams,
			FCollisionShape::MakeBox(FVector(TransformData.VoxelSize / 2.0f)),
			FCollisionQueryParams::DefaultQueryParam))
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				if (AArtifact* Artifact = Cast<AArtifact>(Result.GetActor()))
				{
					HitArtifacts.Add(Artifact);
				}
			}
		}
	}
	OutHitArtifacts = HitArtifacts.Array();
}

void ADigSite::UnDigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius)
{
	SCOPED_NAMED_EVENT(DigSite_UnDigVoxelsInRadius, FColorList::CadetBlue)

	NSVE::FVoxelChunk::FTransformData TransformData = Chunk.MakeTransformData();
	Chunk.Voxels.Iterate([&TransformData, &Location, DigRadius](NSVE::FVoxel& Voxel, int32 Index, const FIntVector& Coords)
	{
		const FVector WorldPosition = NSVE::FVoxelChunk::CoordsToWorld_Static(Coords, TransformData);
		const bool bIsInRadius = Utilities::IsPointInSphere(WorldPosition, Location, DigRadius);
		check((bIsInRadius & 1) == bIsInRadius);

		Voxel.bSolid |= bIsInRadius;
	});
}

void ADigSite::InitializeVoxelGrid()
{
	check(VoxelGrid);

	NSVE::FVoxelGridInitializer Initializer;
	Initializer.Transform = GetTransform();
	Initializer.Bounds = FBoxSphereBounds(Initializer.Transform.GetLocation(), Extents, Extents.Size());
	if (!GeneratedLayers.IsEmpty())
	{
		const double BaseZ = Initializer.Transform.GetLocation().Z;
		for (int32 I = 0; I < GeneratedLayers.Num(); ++I)
		{
			const FVoxelGridGeneratedLayer& Layer = GeneratedLayers[I];
			NSVE::FVoxelLayer& VoxelLayer = Initializer.Layers.AddDefaulted_GetRef();
			VoxelLayer.PlaneMaxZ = BaseZ + Layer.Depth;
			VoxelLayer.LocalMaterialIndex = I;
		}
	}
	else
	{
		Initializer.FillSurfaceZ_WS = Initializer.Transform.GetLocation().Z;
	}

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps,
		Initializer.Transform.GetLocation(),
		Initializer.Transform.GetRotation(),
		QueryParams,
		FCollisionShape::MakeBox(Extents));

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (const AArtifact* Artifact = Cast<AArtifact>(Overlap.GetActor()))
		{
			Initializer.ArtifactLocations.Add(Artifact->GetActorLocation());
			FVector Origin;
			FVector Extent;
			Artifact->GetActorBounds(false, Origin, Extent);
			Initializer.ArtifactRadii.Add(FMath::Max(Extent.Length(), 30.0f));
		}
	}

	VoxelGrid->InitializeGrid(Initializer);
}

void ADigSite::SpawnArtifacts()
{
	const UDataTable* ArtifactDatabase = GetWorld()->GetAuthGameMode<ADr_JonesGameModeBase>()->GetArtifactDataBase();
	if (!ArtifactDatabase)
	{
		return;
	}

	const auto ArtifactMap = ArtifactDatabase->GetRowMap();
	TArray<FName> ArtifactNameArray;
	ArtifactMap.GenerateKeyArray(ArtifactNameArray);

	for (int i = 0; i <= ArtifactSpawnRate; i++)
	{
		FName ArtifactID = ArtifactNameArray[FMath::RandRange(0, ArtifactNameArray.Num() - 1)];

		AArtifact* SpawnedArtifact = UArtifactFactory::ConstructArtifactFromDatabase(*this, ArtifactID);
		SpawnedArtifact->SetActorRotation(FRotator(FMath::RandRange(0.0, 360.0)));

		FVector SpawnExtent = Extents - FVector(100, 100, 0);
		SpawnExtent.X = FMath::Max(SpawnExtent.X, 100);
		SpawnExtent.Y = FMath::Max(SpawnExtent.Y, 100);
		SpawnExtent.Z /= 2.0;

		const FVector DigSiteLocation = GetActorLocation();
		FVector BBoxLocation = DigSiteLocation;
		BBoxLocation.Z -= SpawnExtent.Z;
		FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(BBoxLocation, SpawnExtent);

		FVector ArtifactOrigin;
		FVector ArtifactExtent;
		SpawnedArtifact->GetActorBounds(false, ArtifactOrigin, ArtifactExtent);

		// Bias the Z location towards the surface
		SpawnLocation.Z = DigSiteLocation.Z - (Extents.Z * FMath::Pow(-(SpawnLocation.Z - DigSiteLocation.Z) / Extents.Z, 4.0));
		// 10 cm margin to make sure it definitely doesn't stick out
		SpawnLocation.Z -= ArtifactExtent.Z + 10;
		SpawnLocation.Z = FMath::Max(SpawnLocation.Z, BBoxLocation.Z - (SpawnExtent.Z - 10));

		SpawnedArtifact->SetActorLocation(SpawnLocation);
	}
}
