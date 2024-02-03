// Property of Night Shift Games, all rights reserved.

#include "VoxelEngine.h"

namespace NSVE
{
	// -------------------------- FVoxelArray ----------------------------------------------------------------------

	FORCENOINLINE void FVoxelArray::Clear()
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Clear, FColorList::White)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Clear"), STAT_VoxelEngine_FVoxelArray_Clear, STATGROUP_VoxelEngine)

		for (int32 Index = 0; Index < ElementCount; ++Index)
		{
			Voxels[Index] = FVoxel{};
		}
	}

	FORCENOINLINE void FVoxelArray::Fill(FVoxel Voxel)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Fill, FColorList::LightBlue)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Fill"), STAT_VoxelEngine_FVoxelArray_Fill, STATGROUP_VoxelEngine)

		for (int32 Index = 0; Index < ElementCount; ++Index)
		{
			Voxels[Index] = Voxel;
		}
	}

	FORCENOINLINE bool FVoxelArray::IsUniformlySolid() const
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IsUniform, FColorList::NeonPink)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IsUniform"), STAT_VoxelEngine_FVoxelArray_IsUniform, STATGROUP_VoxelEngine)

		const FVoxel FirstVoxel = Voxels[0];
		for (int32 Index = 1; Index < ElementCount; ++Index)
		{
			if (Voxels[Index].bSolid != FirstVoxel.bSolid)
			{
				return false;
			}
		}
		return true;
	}

	// -------------------------- FVoxelChunk ----------------------------------------------------------------------

	FVoxelChunk::FVoxelChunk(const FVoxelChunkBounds& Bounds) :
		Bounds(Bounds)
	{
	}

	void FVoxelChunk::Clear()
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelChunk_Clear, FColorList::BlueViolet)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelChunk::Clear"), STAT_VoxelEngine_FVoxelChunk_Clear, STATGROUP_VoxelEngine)

		Voxels.Clear();
	}

	void FVoxelChunk::FillTest()
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelChunk_FillTest, FColorList::BlueViolet)

		Voxels.Clear();
		Voxels.Iterate([&](FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		{
			Voxel.bSolid = Coords.Z < 3;
			Voxel.LocalMaterial = 0;
		});

		Voxels.Fill(FVoxel{});
		Voxels.Fill(FVoxel{});
		Voxels.Fill(FVoxel{});
		Voxels.Clear();
		Voxels.Clear();
		Voxels.Clear();
		Voxels.Fill(FVoxel{});
		Voxels.Fill(FVoxel{});
		Voxels.Fill(FVoxel{});
		Voxels.Fill(FVoxel{ true });

		FVoxelArray TestArray;

		TestArray.Fill(FVoxel{ true, 0 });
		Voxels.Union(TestArray);

		Voxels.Clear();

		TestArray.FillTestCube();
		Voxels.Union(TestArray);

		TestArray.FillTestCube({ 5, 5, 5 });
		Voxels.Difference(TestArray);
	}

	void FVoxelChunk::FillSurface(float SurfaceZ_WS)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelChunk_FillSurface, FColorList::BlueViolet)

		const FTransformData TransformData = MakeTransformData();

		Voxels.Clear();
		Voxels.Iterate([&](FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		{
			const FVector WorldPosition = CoordsToWorld_Static(Coords, TransformData);
			Voxel.bSolid = WorldPosition.Z <= SurfaceZ_WS;
			Voxel.LocalMaterial = 0;
		});
	}

	void FVoxelChunk::FillLayered(TArray<FVoxelLayer> VoxelLayers)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelChunk_FillSurface, FColorList::BlueViolet)

		const FTransformData TransformData = MakeTransformData();

		VoxelLayers.Sort([](const FVoxelLayer& Lhs, const FVoxelLayer& Rhs)
		{
			return Lhs.PlaneMaxZ > Rhs.PlaneMaxZ;
		});

		Voxels.Clear();
		for (const FVoxelLayer& Layer : VoxelLayers)
		{
			Voxels.Iterate([&](FVoxel& Voxel, int32 Index, const FIntVector& Coords)
			{
				const FVector WorldPosition = CoordsToWorld_Static(Coords, TransformData);
				if (WorldPosition.Z <= Layer.PlaneMaxZ)
				{
					Voxel.bSolid = true;
					Voxel.LocalMaterial = Layer.LocalMaterialIndex;
				}
			});
		}
	}

#if ENABLE_VOXEL_ENGINE_DEBUG
	void FVoxelChunk::DrawDebugVoxels() const
	{
		if (!GWorld)
		{
			return;
		}

		const FTransformData TransformData = MakeTransformData();

		Voxels.Iterate([&TransformData](const FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		{
			DrawDebugPoint(GWorld, CoordsToWorld_Static(Coords, TransformData), 4.0f, Voxel.bSolid ? FColor::Green : FColor::Red);
		});
	}
#endif

	// -------------------------- FVoxelGrid -----------------------------------------------------------------------

	FVoxelGrid::FVoxelGrid()
		: DimensionsInChunks()
	{
	}

	void FVoxelGrid::Initialize(const FVoxelGridInitializer& Initializer)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelGrid_Initialize, FColorList::GreenCopper)

		Transform = Initializer.Transform;

		DimensionsInChunks.X = FMath::CeilToInt(Initializer.Bounds.BoxExtent.X * 2.0 / FVoxelEngineConfig::ChunkSize);
		DimensionsInChunks.Y = FMath::CeilToInt(Initializer.Bounds.BoxExtent.Y * 2.0 / FVoxelEngineConfig::ChunkSize);
		DimensionsInChunks.Z = FMath::CeilToInt(Initializer.Bounds.BoxExtent.Z * 2.0 / FVoxelEngineConfig::ChunkSize);

		Extent = Initializer.Bounds.BoxExtent;

		Chunks.Reserve(DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z);

		for (int32 Z = 0; Z < DimensionsInChunks.Z; ++Z)
		{
			for (int32 Y = 0; Y < DimensionsInChunks.Y; ++Y)
			{
				for (int32 X = 0; X < DimensionsInChunks.X; ++X)
				{
					FVoxelChunkBounds Bounds = CalcChunkWorldBoundsFromGridCoords(FIntVector{X, Y, Z});
					FVoxelChunk& Chunk = Chunks.Emplace_GetRef(Bounds);
				}
			}
		}

		const int32 ChunkCount = DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z;
		if (!Initializer.Layers.IsEmpty())
		{
			ParallelForTemplate(ChunkCount, [this, &Initializer](int32 Index)
			{
				check(Chunks.IsValidIndex(Index));
				Chunks[Index].FillLayered(Initializer.Layers);

				const FVoxelChunk::FTransformData TransformData = Chunks[Index].MakeTransformData();
				Chunks[Index].Voxels.Iterate([&](FVoxel& Voxel, int32 Index, const FIntVector& Coords)
				{
					const FVector WorldPosition = FVoxelChunk::CoordsToWorld_Static(Coords, TransformData);
					for (const FVector& ArtifactLocation : Initializer.ArtifactLocations)
					{
						static constexpr double MaxDistanceToArtifact = 100.0;
						if ((ArtifactLocation - WorldPosition).SizeSquared() < (MaxDistanceToArtifact * MaxDistanceToArtifact))
						{
							Voxel.LocalMaterial = 0b111;
						}
					}
				});
			});
		}
		else
		{
			ParallelForTemplate(ChunkCount, [this, &Initializer](int32 Index)
			{
				check(Chunks.IsValidIndex(Index));
				Chunks[Index].FillSurface(Initializer.FillSurfaceZ_WS);
			});
		}
	}

	FVoxel* FVoxelGrid::ResolveAddress(const FVoxelAddress& VoxelAddress)
	{
		if (!AreCoordsValid(VoxelAddress.ChunkCoords))
		{
			return nullptr;
		}

		FVoxelChunk& Chunk = GetChunkRefByIndex(CoordsToIndex(VoxelAddress.ChunkCoords));
		if (!Chunk.AreCoordsValid(VoxelAddress.VoxelCoords))
		{
			return nullptr;
		}

		return &Chunk.Voxels[Chunk.CoordsToIndex(VoxelAddress.VoxelCoords)];
	}

	const FVoxel* FVoxelGrid::ResolveAddress(const FVoxelAddress& VoxelAddress) const
	{
		return const_cast<FVoxelGrid*>(this)->ResolveAddress(VoxelAddress);
	}

#if ENABLE_VOXEL_ENGINE_DEBUG
	void FVoxelGrid::DrawDebugChunks(const UWorld& World, const FVector& PositionInside, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness) const
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelGrid_DrawDebugChunks, FColorList::Red)

		const FIntVector GridPosition = CalcGridCoordsFromLocalPosition(PositionInside);
		GEngine->AddOnScreenDebugMessage(213769420, 2.0f, FColor::Yellow, FString::Printf(TEXT("Voxel Grid Location: %s"), *GridPosition.ToString()));

		for (auto It = Chunks.CreateConstIterator(); It; ++It)
		{
			const int32 ChunkIndex = It.GetIndex();
			FVoxelChunkBounds Bounds = CalcChunkWorldBoundsFromIndex(ChunkIndex);
			const bool bIsInside = Bounds.GetBox().IsInside(PositionInside);

			constexpr double ExtentOffsetForDebug = 0.2;
			DrawDebugBox(&World, Bounds.Origin, FVector(Bounds.Extent) - ExtentOffsetForDebug, bIsInside ? FColor::Green : FColor::Red, bPersistentLines, LifeTime, DepthPriority, Thickness);
		}

		const double OuterThicknessOffset = FMath::Max(Thickness, 1.5);
		DrawDebugBox(&World, Transform.GetLocation(), Extent, FColor::Red, bPersistentLines, LifeTime, DepthPriority, Thickness + OuterThicknessOffset);
	}
#endif
}
