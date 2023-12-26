// Property of Night Shift Games, all rights reserved.

#pragma once

#include "MarchingCubes.h"
#include "VoxelEngine.h"

namespace NSVE::Triangulation
{
	namespace Private
	{
		struct FChunkNeighbors
		{
			const FVoxelChunk* _Reserved;
			const FVoxelChunk* X1   = nullptr;
			const FVoxelChunk* Y1   = nullptr;
			const FVoxelChunk* XY1  = nullptr;
			const FVoxelChunk* Z1   = nullptr;
			const FVoxelChunk* XZ1  = nullptr;
			const FVoxelChunk* YZ1  = nullptr;
			const FVoxelChunk* XYZ1 = nullptr;

			const FVoxelChunk* const* AsArray() const
			{
				return reinterpret_cast<const FVoxelChunk* const*>(this);
			}
		};

		inline FChunkNeighbors MakeNeighborsOfChunk(const FVoxelGrid& Grid, int32 ChunkIndex)
		{
			FChunkNeighbors Neighbors;
			const FIntVector Coords = Grid.IndexToCoords(ChunkIndex);
			const FIntVector& GridDimensions = Grid.GetDimensionsInChunks();
			if (Coords.X + 1 < GridDimensions.X)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X + 1, Coords.Y, Coords.Z);
				Neighbors.X1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.X1);
			}
			if (Coords.Y + 1 < GridDimensions.Y)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X, Coords.Y + 1, Coords.Z);
				Neighbors.Y1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.Y1);
			}
			if (Coords.X + 1 < GridDimensions.X && Coords.Y + 1 < GridDimensions.Y)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X + 1, Coords.Y + 1, Coords.Z);
				Neighbors.XY1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.XY1);
			}
			if (Coords.Z + 1 < GridDimensions.Z)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X, Coords.Y, Coords.Z + 1);
				Neighbors.Z1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.Z1);
			}
			if (Coords.X + 1 < GridDimensions.X && Coords.Z + 1 < GridDimensions.Z)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X + 1, Coords.Y, Coords.Z + 1);
				Neighbors.XZ1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.XZ1);
			}
			if (Coords.Y + 1 < GridDimensions.Y && Coords.Z + 1 < GridDimensions.Z)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X, Coords.Y + 1, Coords.Z + 1);
				Neighbors.YZ1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.YZ1);
			}
			if (Coords.X + 1 < GridDimensions.X && Coords.Y + 1 < GridDimensions.Y && Coords.Z + 1 < GridDimensions.Z)
			{
				const FIntVector NeighborCoords = FIntVector(Coords.X + 1, Coords.Y + 1, Coords.Z + 1);
				Neighbors.XYZ1 = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
				check(Neighbors.XYZ1);
			}
			return Neighbors;
		}
	}

	template <typename FInsertVertexFunc, typename FInsertTriangleFunc>
	bool TriangulateCell_MarchingCubes(
		const MarchingCubes::FGridCell& GridCell,
		int32 CurrentIndex,
		FInsertVertexFunc InsertVertexFunc,
		FInsertTriangleFunc InsertTriangleFunc,
		int32& OutVertexCount,
		int32& OutTriangleCount)
	{
		using namespace MarchingCubes;

		// Data local to the current cell
		FVector Vertices[12];
		FTriangle Triangles[5];
		if (!TriangulateGridCell(GridCell, Vertices, Triangles, OutVertexCount, OutTriangleCount))
		{
			return false;
		}

		{
			// SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes_InsertVertices, FColorList::IndianRed)
			for (int32 I = 0; I < OutVertexCount; ++I)
			{
				InsertVertexFunc(Vertices[I]);
			}
		}

		{
			// SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes_InsertTriangles, FColorList::MediumBlue)
			for (int32 I = 0; I < OutTriangleCount; ++I)
			{
				// The triangles' indices need to be offset from the local cell's space.
				Triangles[I].Indices[0] += CurrentIndex;
				Triangles[I].Indices[1] += CurrentIndex;
				Triangles[I].Indices[2] += CurrentIndex;
				InsertTriangleFunc(Triangles[I]);
			}
		}

		return true;
	}

	template <typename FInsertVertexFunc, typename FInsertTriangleFunc>
	void TriangulateVoxelArray_MarchingCubes(
		const FVoxelArray& VoxelArray,
		const FVoxelChunk::FTransformData& TransformData,
		const Private::FChunkNeighbors& Neighbors,
		FInsertVertexFunc InsertVertexFunc,
		FInsertTriangleFunc InsertTriangleFunc)
	{
		using namespace MarchingCubes;

		SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes, FColorList::Feldspar)

		// Uniform array will never produce any vertices
		// TODO: Neighbors will screw this up
		if (VoxelArray.IsUniform())
		{
			return;
		}

		const FIntVector3 Dimensions = VoxelArray.GetDimensions();

		auto ResolveVoxel = [&](const FIntVector& VoxelCoords) -> const FVoxel*
		{
			const bool bRequiresNeighbor = VoxelCoords.X >= Dimensions.X || VoxelCoords.Y >= Dimensions.Y || VoxelCoords.Z >= Dimensions.Z;
			if (!bRequiresNeighbor)
			{
				return &VoxelArray.GetAtCoords(VoxelCoords);
			}

			const int32 NeighborIndex =
				(VoxelCoords.X >= Dimensions.X) |
				(VoxelCoords.Y >= Dimensions.Y) << 1 |
				(VoxelCoords.Z >= Dimensions.Z) << 2;
			if (!ensure(NeighborIndex != 0))
			{
				return nullptr;
			}

			const FVoxelChunk* NeighborChunk = Neighbors.AsArray()[NeighborIndex];
			// Means there's physically no neighbor
			if (!NeighborChunk)
			{
				return nullptr;
			}

			const FIntVector TransformedCoords = FIntVector(VoxelCoords.X % Dimensions.X, VoxelCoords.Y % Dimensions.Y, VoxelCoords.Z % Dimensions.Z);
			return &NeighborChunk->Voxels.GetAtCoords(TransformedCoords);
		};

		int32 LastIndex = 0;
		for (int32 Z = 0; Z < Dimensions.Z; ++Z)
		{
			for (int32 Y = 0; Y < Dimensions.Y; ++Y)
			{
				for (int32 X = 0; X < Dimensions.X; ++X)
				{
					// SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes_TriangulateCell, FColorList::Bronze)

					const FIntVector3 GridCellCornerCoords[8] = {
						FIntVector3{ X,     Y,     Z     },
						FIntVector3{ X + 1, Y,     Z     },
						FIntVector3{ X + 1, Y + 1, Z     },
						FIntVector3{ X,     Y + 1, Z     },
						FIntVector3{ X,     Y,     Z + 1 },
						FIntVector3{ X + 1, Y,     Z + 1 },
						FIntVector3{ X + 1, Y + 1, Z + 1 },
						FIntVector3{ X,     Y + 1, Z + 1 },
					};

					FGridCell GridCell;
					for (int32 Corner = 0; Corner < 8; ++Corner)
					{
						const FIntVector& VoxelCoords = GridCellCornerCoords[Corner];
						const FVoxel* Voxel = ResolveVoxel(VoxelCoords);
						if (!Voxel)
						{
							continue;
						}
						GridCell.Values[Corner] = Voxel->bSolid ? -1.0f : 1.0f;
						GridCell.Positions[Corner] = FVoxelChunk::GridPositionToWorld_Static(GridCellCornerCoords[Corner], TransformData);
					}

					int32 VertexCount;
					int32 TriangleCount;
					if (!TriangulateCell_MarchingCubes(GridCell, LastIndex, InsertVertexFunc, InsertTriangleFunc, VertexCount, TriangleCount))
					{
						continue;
					}

					LastIndex += VertexCount;
				}
			}
		}
	}

	template <typename FInsertVertexFunc, typename FInsertTriangleFunc>
	void TriangulateVoxelChunk_MarchingCubes(
		const FVoxelChunk& VoxelChunk,
		const Private::FChunkNeighbors& Neighbors,
		FInsertVertexFunc InsertVertexFunc,
		FInsertTriangleFunc InsertTriangleFunc)
	{
		TriangulateVoxelArray_MarchingCubes(VoxelChunk.Voxels, VoxelChunk.MakeTransformData(), Neighbors, InsertVertexFunc, InsertTriangleFunc);
	}

	inline void TriangulateVoxelGrid_MarchingCubes(
		const FVoxelGrid& VoxelGrid,
		TArray<FVector>& OutCombinedVertices,
		TArray<MarchingCubes::FTriangle>& OutCombinedTriangles,
		int32& OutVertexCount,
		int32& OutTriangleCount)
	{
		using namespace MarchingCubes;
		using namespace Private;

		SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelGrid_MarchingCubes, FColorList::Wheat)

		OutVertexCount = 0;
		OutTriangleCount = 0;

		int32 CombinedVertexCount = 0;
		OutCombinedVertices.Reserve(1000 * VoxelGrid.GetChunkCount());
		OutCombinedTriangles.Reserve(1000 * VoxelGrid.GetChunkCount());

		FCriticalSection TransactionGuard;

		VoxelGrid.IterateChunks_Parallel([&](const FVoxelChunk& Chunk, int32 Index)
		{
			static constexpr int32 InitialArraySize = 1000;

			TArray<FVector> Vertices;
			TArray<FTriangle> Triangles;
			Vertices.Reserve(InitialArraySize);
			Triangles.Reserve(InitialArraySize);

			const FChunkNeighbors Neighbors = MakeNeighborsOfChunk(VoxelGrid, Index);
			auto InsertVertexFn   = [&Vertices] (const FVector& Vertex)     { Vertices.Add(Vertex);    };
			auto InsertTriangleFn = [&Triangles](const FTriangle& Triangle) { Triangles.Add(Triangle); };
			TriangulateVoxelChunk_MarchingCubes(Chunk, Neighbors, InsertVertexFn, InsertTriangleFn);

			const int32 VertexCount = Vertices.Num();
			{
				FScopeLock Lock(&TransactionGuard);

				Algo::Copy(Vertices, OutCombinedVertices);
				Algo::Transform(Triangles, OutCombinedTriangles, [&](FTriangle Triangle)
				{
					Triangle.Indices[0] += CombinedVertexCount;
					Triangle.Indices[1] += CombinedVertexCount;
					Triangle.Indices[2] += CombinedVertexCount;
					return Triangle;
				});
				CombinedVertexCount += VertexCount;
			}
		});

		OutVertexCount = CombinedVertexCount;
		OutTriangleCount = OutCombinedTriangles.Num();
	}
}
