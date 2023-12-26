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
			static constexpr int32 Count = 8;

			union
			{
				struct
				{
					const FVoxelChunk* _Reserved;
					const FVoxelChunk* X1;
					const FVoxelChunk* Y1;
					const FVoxelChunk* XY1;
					const FVoxelChunk* Z1;
					const FVoxelChunk* XZ1;
					const FVoxelChunk* YZ1;
					const FVoxelChunk* XYZ1;
				};
				/**
				 * The neighbors can be indexed in a bitfield style,
				 * where each bit corresponds to each dimension.
				 *
				 * 1 << 0  =>  X
				 * 1 << 1  =>  Y
				 * 1 << 2  =>  Z
				 * 
				 * e.g. 011 (3) - this would be the XY neighbor
				 *
				 * This cool little trick is possible, because we only
				 * need to have access to the positive offset neighbors.
				 */
				const FVoxelChunk* Chunks[Count];
			};

			const FVoxelChunk* AtIndex(int32 Index) const
			{
				// NOTE: 0th index is not a neighbor at all
				check(Index > 0 && Index < Count);
				return Chunks[Index];
			}

			const FVoxelChunk* operator[](int32 Index) const
			{
				return AtIndex(Index);
			}
		};

		inline FChunkNeighbors MakeNeighborsOfChunk(const FVoxelGrid& Grid, int32 ChunkIndex)
		{
			FChunkNeighbors Neighbors;
			const FIntVector Coords = Grid.IndexToCoords(ChunkIndex);
			const FIntVector& GridDimensions = Grid.GetDimensionsInChunks();
			for (int32 N = 1; N < FChunkNeighbors::Count; ++N)
			{
				const int32 OX = (N & (1 << 0)) > 0;
				const int32 OY = (N & (1 << 1)) > 0;
				const int32 OZ = (N & (1 << 2)) > 0;
				if (Coords.X + OX >= GridDimensions.X ||
					Coords.Y + OY >= GridDimensions.Y ||
					Coords.Z + OZ >= GridDimensions.Z)
				{
					Neighbors.Chunks[N] = nullptr;
					continue;
				}
				const FIntVector NeighborCoords = FIntVector(Coords.X + OX, Coords.Y + OY, Coords.Z + OZ);
				Neighbors.Chunks[N] = Grid.GetChunkByIndex(Grid.CoordsToIndex(NeighborCoords));
			}
			return Neighbors;
		}

		inline bool IsVoxelArrayUniformlySolid_NeighborFix(const FVoxelArray& VoxelArray, const FChunkNeighbors& Neighbors)
		{
			const FVoxel FirstVoxel = VoxelArray[0];
			const FIntVector Dimensions = VoxelArray.GetDimensions();
			for (int32 Z = 0; Z < Dimensions.Z + 1; ++Z)
			{
				for (int32 Y = 0; Y < Dimensions.Y + 1; ++Y)
				{
					for (int32 X = 0; X < Dimensions.X + 1; ++X)
					{
						FVoxel Voxel;
						const int32 NeighborIndex =
							X / Dimensions.X << 0 |
							Y / Dimensions.Y << 1 |
							Z / Dimensions.Z << 2;
						if (NeighborIndex > 0)
						{
							const FVoxelChunk* Neighbor = Neighbors.Chunks[NeighborIndex];
							if (Neighbor == nullptr)
							{
								continue;
							}
							Voxel = Neighbor->Voxels.GetAtCoords(FIntVector(X % Dimensions.X, Y % Dimensions.Y, Z % Dimensions.Z));
						}
						else
						{
							Voxel = VoxelArray.GetAtCoords(FIntVector(X, Y, Z));
						}

						if (Voxel.bSolid != FirstVoxel.bSolid)
						{
							return false;
						}
					}
				}
			}
			return true;
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
		using namespace Private;

		SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes, FColorList::Feldspar)

		// Uniform array will never produce any vertices
		if (IsVoxelArrayUniformlySolid_NeighborFix(VoxelArray, Neighbors))
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
				(VoxelCoords.X >= Dimensions.X) << 0 |
				(VoxelCoords.Y >= Dimensions.Y) << 1 |
				(VoxelCoords.Z >= Dimensions.Z) << 2;
			if (!ensure(NeighborIndex != 0))
			{
				return nullptr;
			}

			const FVoxelChunk* NeighborChunk = Neighbors.Chunks[NeighborIndex];
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
