// Property of Night Shift Games, all rights reserved.

#pragma once

#include "MarchingCubes.h"
#include "VoxelEngine.h"

namespace NSVE::Triangulation
{
	template <typename FInsertVertexFunc, typename FInsertTriangleFunc>
	void TriangulateVoxelArray_MarchingCubes(const FVoxelArray& VoxelArray, const FVoxelChunk::FLocalToWorldTransformData& TransformData, FInsertVertexFunc InsertVertexFunc, FInsertTriangleFunc InsertTriangleFunc)
	{
		using namespace MarchingCubes;

		SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes, FColorList::Feldspar)

		const FIntVector3 Dimensions = VoxelArray.GetDimensions();

		int32 LastIndex = 0;
		for (int32 Z = 0; Z < Dimensions.Z - 1; ++Z)
		{
			for (int32 Y = 0; Y < Dimensions.Y - 1; ++Y)
			{
				for (int32 X = 0; X < Dimensions.X - 1; ++X)
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
						const FVoxel& Voxel = VoxelArray.GetAtCoords(GridCellCornerCoords[Corner]);
						GridCell.Values[Corner] = Voxel.bSolid ? -1.0f : 1.0f;
						GridCell.Positions[Corner] = FVoxelChunk::LocalPositionToWorld_Static(GridCellCornerCoords[Corner], TransformData);
					}

					// Data local to the current cell
					FVector Vertices[12];
					FTriangle Triangles[5];
					int32 VertexCount;
					int32 TriangleCount;
					if (!TriangulateGridCell(GridCell, Vertices, Triangles, VertexCount, TriangleCount))
					{
						continue;
					}

					{
						// SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes_InsertVertices, FColorList::IndianRed)
						for (int32 I = 0; I < VertexCount; ++I)
						{
							InsertVertexFunc(Vertices[I]);
						}
					}

					{
						// SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelArray_MarchingCubes_InsertTriangles, FColorList::MediumBlue)
						for (int32 I = 0; I < TriangleCount; ++I)
						{
							// The triangles' indices need to be offset from the local cell's space.
							Triangles[I].Indices[0] += LastIndex;
							Triangles[I].Indices[1] += LastIndex;
							Triangles[I].Indices[2] += LastIndex;
							InsertTriangleFunc(Triangles[I]);
						}
					}

					LastIndex += VertexCount;
				}
			}
		}
	}

	template <typename FInsertVertexFunc, typename FInsertTriangleFunc>
	void TriangulateVoxelChunk_MarchingCubes(const FVoxelChunk& VoxelChunk, FInsertVertexFunc InsertVertexFunc, FInsertTriangleFunc InsertTriangleFunc)
	{
		TriangulateVoxelArray_MarchingCubes(VoxelChunk.Voxels, VoxelChunk.MakeLocalToWorldTransformData(), InsertVertexFunc, InsertTriangleFunc);
	}

	inline void TriangulateVoxelGrid_MarchingCubes(const FVoxelGrid& VoxelGrid, TArray<FVector>& OutCombinedVertices, TArray<MarchingCubes::FTriangle>& OutCombinedTriangles, int32& OutVertexCount, int32& OutTriangleCount)
	{
		using namespace MarchingCubes;

		SCOPED_NAMED_EVENT(VoxelEngine_Triangulation_TriangulateVoxelGrid_MarchingCubes, FColorList::Wheat)

		OutVertexCount = 0;
		OutTriangleCount = 0;

		TAtomic<int32> CombinedVertexCount = 0;

		OutCombinedVertices.Reserve(1000 * VoxelGrid.GetChunkCount());
		OutCombinedTriangles.Reserve(1000 * VoxelGrid.GetChunkCount());
		FCriticalSection CombinedVerticesGuard;
		FCriticalSection CombinedTrianglesGuard;

		VoxelGrid.IterateChunks_Parallel([&](const FVoxelChunk& Chunk, int32 Index)
		{
			static constexpr int32 InitialArraySize = 1000;

			TArray<FVector> Vertices;
			TArray<FTriangle> Triangles;
			Vertices.Reserve(InitialArraySize);
			Triangles.Reserve(InitialArraySize);

			auto InsertVertexFn = [&Vertices](const FVector& Vertex)
			{
				Vertices.Add(Vertex);
			};
			auto InsertTriangleFn = [&Triangles](const FTriangle& Triangle)
			{
				Triangles.Add(Triangle);
			};
			TriangulateVoxelChunk_MarchingCubes(Chunk, InsertVertexFn, InsertTriangleFn);

			const int32 VertexCount = Vertices.Num();
			const int32 PreviousVertexCount = CombinedVertexCount.AddExchange(VertexCount);

			{
				FScopeLock Lock(&CombinedVerticesGuard);
				Algo::Copy(Vertices, OutCombinedVertices);
			}

			{
				FScopeLock Lock(&CombinedTrianglesGuard);
				Algo::Transform(Triangles, OutCombinedTriangles, [&](FTriangle Triangle)
				{
					Triangle.Indices[0] += PreviousVertexCount;
					Triangle.Indices[1] += PreviousVertexCount;
					Triangle.Indices[2] += PreviousVertexCount;
					return Triangle;
				});
			}
		});

		OutVertexCount = CombinedVertexCount;
		OutTriangleCount = OutCombinedTriangles.Num();
	}
}
