// Property of Night Shift Games, all rights reserved.

#pragma once

#include "IndexTypes.h"
#include "VoxelEngine.h"

namespace NS::SurfaceNets
{
	using namespace NSVE;
	using namespace UE::Geometry;

	namespace Debug
	{
		inline TAutoConsoleVariable CVar_SurfacePoints(
			TEXT("NS.VE.SurfaceNets.Debug.SurfacePoints"),
			false,
			TEXT("Voxel Engine Surface Nets - enable surface points debug visualization."),
			ECVF_Cheat);

		inline TAutoConsoleVariable CVar_Cells(
			TEXT("NS.VE.SurfaceNets.Debug.Cells"),
			false,
			TEXT("Voxel Engine Surface Nets - enable cells debug visualization."),
			ECVF_Cheat);

		struct FSurfacePointVis
		{
			int32 ChunkIndex;
			FIntVector CellCoords;
			FVector Location;
			int32 Index;
			uint8 EdgeIntersections;
		};

		struct FCellVis
		{
			FIntVector CellCoords;
		};

		struct FDebugContext
		{
			TArray<FSurfacePointVis> SurfacePoints;
			FCriticalSection SurfacePointsMutex;
		};

		inline void ClearDebugContext(FDebugContext& DebugContext)
		{
			DebugContext.SurfacePoints.Reset();
		}
	}

	struct FCell
	{
		FVector Positions[8];
		float Values[8];
	};

	enum ECorner : uint8
	{
		Corner_Zero = 0b000,
		Corner_X    = 0b001,
		Corner_Y    = 0b010,
		Corner_XY   = 0b011,
		Corner_Z    = 0b100,
		Corner_XZ   = 0b101,
		Corner_YZ   = 0b110,
		Corner_XYZ  = 0b111,
	};

	inline bool IsCellEdgeIntersectingSurface(const FCell& Cell, ECorner CornerA, ECorner CornerB, bool* bOutCornerBInside)
	{
		check(CornerA < 8 && CornerB < 8);
		const bool bIntersects = Cell.Values[CornerA] > 0.0f != Cell.Values[CornerB] > 0.0f;
		if (bIntersects && bOutCornerBInside)
		{
			*bOutCornerBInside = Cell.Values[CornerB] <= 0.0f;
		}
		return bIntersects;
	}

	inline bool IsCellIntersectingSurface(const FCell& Cell)
	{
		const bool bSign = Cell.Values[0] > 0.0f;
		for (int32 CI = 1; CI < 8; ++CI)
		{
			if (Cell.Values[CI] > 0.0f != bSign)
			{
				return true;
			}
		}
		return false;
	}

	inline FVector FindSurfacePointInCell(const FCell& Cell)
	{
		check(IsCellIntersectingSurface(Cell));
		// TODO: When we have SDF-like thing, this needs to be changed to an edge intersection based average
		FVector AveragePosition = FVector::ZeroVector;
		for (int32 CI = 0; CI < 8; ++CI)
		{
			AveragePosition += Cell.Positions[CI];
		}
		AveragePosition /= 8.0;
		return AveragePosition;
	}

	inline bool MakeCellAtVoxel(
		const FVoxelGrid& Grid,
		const FVoxelChunk& Chunk,
		int32 ChunkIndex,
		const FVoxelChunk::FTransformData& ChunkTransformData,
		const FIntVector& VoxelCoords,
		FCell& OutCell)
	{
		for (int32 CI = 0; CI < 8; ++CI)
		{
			FIntVector CornerCoords;
			CornerCoords.X = VoxelCoords.X + !!(CI & 1 << 0);
			CornerCoords.Y = VoxelCoords.Y + !!(CI & 1 << 1);
			CornerCoords.Z = VoxelCoords.Z + !!(CI & 1 << 2);
			// Corner coords may be outside the current chunk
			const bool bXOut = CornerCoords.X >= FVoxelChunk::Resolution;
			const bool bYOut = CornerCoords.Y >= FVoxelChunk::Resolution;
			const bool bZOut = CornerCoords.Z >= FVoxelChunk::Resolution;
			const bool bIsCornerOutsideChunk = bXOut || bYOut || bZOut;

			const FVoxelChunk* ActualChunk;
			if (!bIsCornerOutsideChunk)
			{
				ActualChunk = &Chunk;
				OutCell.Positions[CI] = FVoxelChunk::CoordsToWorld_Static(CornerCoords, ChunkTransformData);
			}
			else
			{
				FIntVector ChunkCoords = Grid.IndexToCoords(ChunkIndex);
				ChunkCoords.X += bXOut;
				ChunkCoords.Y += bYOut;
				ChunkCoords.Z += bZOut;
				if (!Grid.AreCoordsValid(ChunkCoords))
				{
					return false;
				}

				ActualChunk = Grid.GetChunkByIndex(Grid.CoordsToIndex(ChunkCoords));
				check(ActualChunk);

				CornerCoords %= FVoxelChunk::Resolution;
				OutCell.Positions[CI] = FVoxelChunk::CoordsToWorld_Static(CornerCoords, ActualChunk->MakeTransformData());
			}

			// TODO: There should probably be more SDF-like info in the voxels than just one bSolid flag
			const FVoxel& Voxel = ActualChunk->Voxels.GetAtCoords(CornerCoords);
			OutCell.Values[CI] = Voxel.bSolid ? -1.0f : 1.0f;
		}
		return true;
	}

	inline bool MakeQuadAtCellEdge(
		const FIntVector(&NeighborCellCoords)[3],
		int32 CellSurfacePointIndex,
		const TMap<FIntVector, int32>& IndexMap,
		bool bWinding,
		FIndex3i* OutTriangles)
	{
		check(CellSurfacePointIndex >= 0);
		check(OutTriangles);

		auto FindIndex = [&](int32 I)
		{
			const FIntVector& CellCoords = NeighborCellCoords[I - 1];
			if (const int32* VP = IndexMap.Find(CellCoords))
			{
				return *VP;
			}
			return -1;
		};
#define _SN_FIND_INDEX(I) FindIndex(I); if (V##I == -1) return false

		const int32 V0 = CellSurfacePointIndex;
		const int32 V1 = _SN_FIND_INDEX(1);
		const int32 V2 = _SN_FIND_INDEX(2);
		const int32 V3 = _SN_FIND_INDEX(3);

#undef _SN_FIND_INDEX

		// TODO: Split the quad along the shorter axis

		if (bWinding)
		{
			OutTriangles[0] = FIndex3i{ V0, V1, V3, };
			OutTriangles[1] = FIndex3i{ V0, V3, V2, };
		}
		else
		{
			OutTriangles[0] = FIndex3i{ V0, V3, V1, };
			OutTriangles[1] = FIndex3i{ V0, V2, V3, };
		}

		return true;
	}

	inline void TriangulateVoxelChunk(
		const FVoxelGrid& VoxelGrid,
		const FVoxelChunk& VoxelChunk,
		int32 ChunkIndex,
		TArray<FVector>& OutVertices,
		TArray<FIndex3i>& OutTriangles,
		Debug::FDebugContext* DebugContext = nullptr)
	{
		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_TriangulateVoxelChunk, FColorList::BrightGold)

		struct FSurfacePoint
		{
			FVector Location;
			FIntVector CellCoords;
			int32 CellIndex;
		};

		const FVoxelChunk::FTransformData TransformData = VoxelChunk.MakeTransformData();
		constexpr int32 CellCount = (FVoxelChunk::Resolution + 1) * (FVoxelChunk::Resolution + 1) * (FVoxelChunk::Resolution + 1);

		TArray<FCell> Cells;
		TArray<FSurfacePoint> SurfacePoints;
		TMap<FIntVector, int32> CoordsToSurfacePointIndexMap;

		Cells.SetNumZeroed(CellCount);
		SurfacePoints.Reserve(CellCount);
		CoordsToSurfacePointIndexMap.Reserve(CellCount);

		int32 CellIndex = 0;
		FIntVector VoxelCoords;
		// NOTE: One more cell per dimension is needed for creating quads at chunk's borders
		for (VoxelCoords.Z = 0; VoxelCoords.Z < FVoxelChunk::Resolution + 1; ++VoxelCoords.Z)
		{
			for (VoxelCoords.Y = 0; VoxelCoords.Y < FVoxelChunk::Resolution + 1; ++VoxelCoords.Y)
			{
				for (VoxelCoords.X = 0; VoxelCoords.X < FVoxelChunk::Resolution + 1; ++VoxelCoords.X, ++CellIndex)
				{
					FCell& Cell = Cells[CellIndex];
					if (!MakeCellAtVoxel(VoxelGrid, VoxelChunk, ChunkIndex, TransformData, VoxelCoords, Cell))
					{
						continue;
					}

					if (!IsCellIntersectingSurface(Cell))
					{
						continue;
					}

					CoordsToSurfacePointIndexMap.Add(VoxelCoords, SurfacePoints.Num());
					FSurfacePoint& SurfacePointRef = SurfacePoints.Emplace_GetRef();
					SurfacePointRef.Location = FindSurfacePointInCell(Cell);
					SurfacePointRef.CellCoords = VoxelCoords;
					SurfacePointRef.CellIndex = CellIndex;
				}
			}
		}

		TArray<Debug::FSurfacePointVis> SurfacePointVises;

		for (auto It = SurfacePoints.CreateConstIterator(); It; ++It)
		{
			const FSurfacePoint& SurfacePoint = *It;
			const FIntVector& CellCoords = SurfacePoint.CellCoords;
			check(CellCoords.X >= 0 && CellCoords.X < FVoxelChunk::Resolution + 1);
			check(CellCoords.Y >= 0 && CellCoords.Y < FVoxelChunk::Resolution + 1);
			check(CellCoords.Z >= 0 && CellCoords.Z < FVoxelChunk::Resolution + 1);
			const int32 SurfacePointIndex = It.GetIndex();

			OutVertices.Add(SurfacePoint.Location);

			FCell& Cell = Cells[SurfacePoint.CellIndex];

			bool bXYZCornerInside;
			uint8 EdgeIntersections = 0;

			// YZ Edge - X Axis
			if (IsCellEdgeIntersectingSurface(Cell, Corner_YZ, Corner_XYZ, &bXYZCornerInside))
			{
				EdgeIntersections |= 1 << 0;

				FIntVector NeighborCellCoords[3];
				NeighborCellCoords[0] = { CellCoords.X, CellCoords.Y + 1, CellCoords.Z };
				NeighborCellCoords[1] = { CellCoords.X, CellCoords.Y, CellCoords.Z + 1 };
				NeighborCellCoords[2] = { CellCoords.X, CellCoords.Y + 1, CellCoords.Z + 1 };

				OutTriangles.AddUninitialized(2);
				FIndex3i* QuadIndexBuffer = OutTriangles.GetData() + OutTriangles.Num() - 2;
				if (!MakeQuadAtCellEdge(NeighborCellCoords, SurfacePointIndex, CoordsToSurfacePointIndexMap, bXYZCornerInside, QuadIndexBuffer))
				{
					OutTriangles.RemoveAt(OutTriangles.Num() - 2, 2);
				}
			}

			// XZ Edge - Y Axis
			if (IsCellEdgeIntersectingSurface(Cell, Corner_XZ, Corner_XYZ, &bXYZCornerInside))
			{
				EdgeIntersections |= 1 << 1;

				FIntVector NeighborCellCoords[3];
				NeighborCellCoords[0] = { CellCoords.X + 1, CellCoords.Y, CellCoords.Z };
				NeighborCellCoords[1] = { CellCoords.X, CellCoords.Y, CellCoords.Z + 1 };
				NeighborCellCoords[2] = { CellCoords.X + 1, CellCoords.Y, CellCoords.Z + 1 };

				OutTriangles.AddUninitialized(2);
				FIndex3i* QuadIndexBuffer = OutTriangles.GetData() + OutTriangles.Num() - 2;
				if (!MakeQuadAtCellEdge(NeighborCellCoords, SurfacePointIndex, CoordsToSurfacePointIndexMap, !bXYZCornerInside, QuadIndexBuffer))
				{
					OutTriangles.RemoveAt(OutTriangles.Num() - 2, 2);
				}
			}

			// XY Edge - Z Axis
			if (IsCellEdgeIntersectingSurface(Cell, Corner_XY, Corner_XYZ, &bXYZCornerInside))
			{
				EdgeIntersections |= 1 << 2;

				FIntVector NeighborCellCoords[3];
				NeighborCellCoords[0] = { CellCoords.X + 1, CellCoords.Y, CellCoords.Z };
				NeighborCellCoords[1] = { CellCoords.X, CellCoords.Y + 1, CellCoords.Z };
				NeighborCellCoords[2] = { CellCoords.X + 1, CellCoords.Y + 1, CellCoords.Z };

				OutTriangles.AddUninitialized(2);
				FIndex3i* QuadIndexBuffer = OutTriangles.GetData() + OutTriangles.Num() - 2;
				if (!MakeQuadAtCellEdge(NeighborCellCoords, SurfacePointIndex, CoordsToSurfacePointIndexMap, bXYZCornerInside, QuadIndexBuffer))
				{
					OutTriangles.RemoveAt(OutTriangles.Num() - 2, 2);
				}
			}

			if (DebugContext)
			{
				Debug::FSurfacePointVis& SurfacePointVis = SurfacePointVises.Emplace_GetRef();
				SurfacePointVis.Location = SurfacePoint.Location;
				SurfacePointVis.Index = It.GetIndex();
				SurfacePointVis.CellCoords = SurfacePoint.CellCoords;
				SurfacePointVis.ChunkIndex = ChunkIndex;
				SurfacePointVis.EdgeIntersections = EdgeIntersections;
			}
		}

		if (DebugContext)
		{
			FScopeLock DebugLock(&DebugContext->SurfacePointsMutex);
			for (auto It = SurfacePointVises.CreateConstIterator(); It; ++It)
			{
				DebugContext->SurfacePoints.Add(*It);
			}
		}
	}

	inline void TriangulateVoxelGrid(
		const FVoxelGrid& VoxelGrid,
		TArray<FVector>& OutVertices,
		TArray<FIndex3i>& OutTriangles,
		Debug::FDebugContext* DebugContext = nullptr)
	{
		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_TriangulateVoxelGrid, FColorList::Wheat)

		if (DebugContext)
		{
			Debug::ClearDebugContext(*DebugContext);
		}

		int32 CombinedVertexCount = 0;
		OutVertices.Reserve(1000 * VoxelGrid.GetChunkCount());
		OutTriangles.Reserve(1000 * VoxelGrid.GetChunkCount());

		FCriticalSection TransactionGuard;

		VoxelGrid.IterateChunks_Parallel([&](const FVoxelChunk& Chunk, int32 ChunkIndex)
		{
			TArray<FVector> Vertices;
			TArray<FIndex3i> Triangles;
			TriangulateVoxelChunk(VoxelGrid, Chunk, ChunkIndex, Vertices, Triangles, DebugContext);

			{
				FScopeLock Lock(&TransactionGuard);

				Algo::Copy(Vertices, OutVertices);
				Algo::Transform(Triangles, OutTriangles, [&](FIndex3i Triangle)
				{
					Triangle.A += CombinedVertexCount;
					Triangle.B += CombinedVertexCount;
					Triangle.C += CombinedVertexCount;
					return Triangle;
				});
				CombinedVertexCount += Vertices.Num();
			}
		});
	}
}
