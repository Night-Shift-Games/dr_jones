﻿// Property of Night Shift Games, all rights reserved.

#include "VoxelEngine.h"

namespace VoxelEngine
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

	FORCENOINLINE void FVoxelArray::Fill(FAlignedVoxel Voxel)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Fill, FColorList::LightBlue)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Fill"), STAT_VoxelEngine_FVoxelArray_Fill, STATGROUP_VoxelEngine)

		for (int32 Index = 0; Index < ElementCount; ++Index)
		{
			Voxels[Index] = Voxel;
		}
	}

	// -------------------------- FVoxelChunk ----------------------------------------------------------------------

	FVoxelChunk::FVoxelChunk(const FVoxelChunkBounds& Bounds) :
		VoxelOctree(Bounds.Origin, Bounds.Extent),
		Bounds(Bounds)
	{
	}

	void FVoxelChunk::FillUniform()
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelChunk_FillUniform, FColorList::BlueViolet)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelChunk::FillUniform"), STAT_VoxelEngine_FVoxelChunk_FillUniform, STATGROUP_VoxelEngine)

		VoxelOctree.Destroy();

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

#if ENABLE_VOXEL_ENGINE_DEBUG
	void FVoxelChunk::DrawDebugVoxels() const
	{
		if (!GWorld)
		{
			return;
		}

		const FLocalToWorldTransformData LocalToWorldTransformData = MakeLocalToWorldTransformData();

		Voxels.Iterate([&LocalToWorldTransformData](const FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		{
			DrawDebugPoint(GWorld, LocalPositionToWorld_Static(Coords, LocalToWorldTransformData), 4.0f, Voxel.bSolid ? FColor::Green : FColor::Red);
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
					FVoxelChunkBounds Bounds = CalcChunkWorldBoundsFromGridPosition(FIntVector{X, Y, Z});
					FVoxelChunk& Chunk = Chunks.Emplace_GetRef(Bounds);
				}
			}
		}

		const int32 ChunkCount = DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z;
		ParallelFor(ChunkCount, [this](int32 Index)
		{
			FVoxelChunk* Chunk;
			{
				FScopeLock Lock(&ChunksGuard);
				Chunk = &Chunks[Index];
			}

			Chunk->FillUniform();
		});
	}

#if ENABLE_VOXEL_ENGINE_DEBUG
	void FVoxelGrid::DrawDebugChunks(const UWorld& World, const FVector& PositionInside, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness) const
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelGrid_DrawDebugChunks, FColorList::Red)

		const FIntVector GridPosition = CalcGridPositionFromLocalPosition(PositionInside);
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
