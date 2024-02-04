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

		inline TAutoConsoleVariable CVar_Cells_Intersections(
			TEXT("NS.VE.SurfaceNets.Debug.Cells.Intersections"),
			false,
			TEXT("Voxel Engine Surface Nets - draw edge surface intersections in cells."),
			ECVF_Cheat);

		inline TAutoConsoleVariable CVar_Cells_SDFValues(
			TEXT("NS.VE.SurfaceNets.Debug.Cells.SDFValues"),
			false,
			TEXT("Voxel Engine Surface Nets - draw SDF values of cell corners."),
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

	constexpr inline ECorner CellEdges[12][2] = {
		{ Corner_Zero, Corner_X },
		{ Corner_Y,    Corner_XY },
		{ Corner_Z,    Corner_XZ },
		{ Corner_YZ,   Corner_XYZ },
		{ Corner_Zero, Corner_Y },
		{ Corner_X,    Corner_XY },
		{ Corner_Z,    Corner_YZ },
		{ Corner_XZ,   Corner_XYZ },
		{ Corner_Zero, Corner_Z },
		{ Corner_X,    Corner_XZ },
		{ Corner_Y,    Corner_YZ },
		{ Corner_XY,   Corner_XYZ },
	};

	// An SDF that allows for triangulating the whole chunk using surface nets without querying the voxel grid.
	struct alignas(Alignment) FExtendedSDF
	{
		// Two samples at the end of each axis are needed for the quad generation.
		// Two additional samples on each side are needed for convolution.
		static constexpr int32 Resolution = FVoxelChunk::Resolution + 6;
		TStaticArray<float, Resolution * Resolution * Resolution> SDF;

		float Get(const FIntVector& Coords) const
		{
			check(Utils::IsInlineGridPositionValid(Coords, Resolution));
			return SDF[Utils::InlineGridPositionToIndex(Coords, Resolution)];
		}

		float GetByVoxelCoords(const FIntVector& VoxelCoords) const
		{
			// SDF's coords start at -2 in relation to its chunk.
			// This is needed to make convolution by a 3x3x3 kernel possible.
			const int32 X = VoxelCoords.X + 2;
			const int32 Y = VoxelCoords.Y + 2;
			const int32 Z = VoxelCoords.Z + 2;
			check(X >= 0 && X < Resolution);
			check(Y >= 0 && Y < Resolution);
			check(Z >= 0 && Z < Resolution);
			const int32 Index = Z * Resolution * Resolution + Y * Resolution + X;
			return SDF[Index];
		}

		template <typename FFunc>
		void Iterate(FFunc ForEach)
		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_ExtendedSDF_Iterate, FColorList::MediumGoldenrod)

			int32 Index = 0;
			FIntVector Coords;
			for (Coords.Z = 0; Coords.Z < Resolution; ++Coords.Z)
			{
				for (Coords.Y = 0; Coords.Y < Resolution; ++Coords.Y)
				{
					for (Coords.X = 0; Coords.X < Resolution; ++Coords.X, ++Index)
					{
						ForEach(SDF[Index], Index, Coords);
					}
				}
			}
		}

		template <typename FFunc>
		void Iterate_Static(FFunc ForEach)
		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_ExtendedSDF_Iterate, FColorList::MediumGoldenrod)

			int32 Index = 0;
			FIntVector Coords;
			for (Coords.Z = 0; Coords.Z < Resolution; ++Coords.Z)
			{
				for (Coords.Y = 0; Coords.Y < Resolution; ++Coords.Y)
				{
					for (Coords.X = 0; Coords.X < Resolution; ++Coords.X, ++Index)
					{
						ForEach(Index, Coords);
					}
				}
			}
		}

		void Reset()
		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_ExtendedSDF_Reset, FColorList::DarkOrchid)

			for (int32 Index = 0; Index < SDF.Num(); ++Index)
			{
				SDF[Index] = NAN;
			}
		}
	};

	inline TUniquePtr<FExtendedSDF> GenerateChunkExtendedSDF(const FVoxelGrid& Grid, int32 ChunkIndex)
	{
		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_GenerateChunkExtendedSDF, FColorList::NeonPink)

		const FIntVector BaseGlobalCoords = Grid.CalcGlobalCoordsFromVoxelAddressChecked(MakeVoxelAddress(Grid.IndexToCoords(ChunkIndex), FIntVector{}));
		TUniquePtr<FExtendedSDF> ExtendedSDF = TUniquePtr<FExtendedSDF>(new FExtendedSDF);
		FIntVector LocalVoxelCoords;
		int32 SDFIndex = 0;
		for (LocalVoxelCoords.Z = -2; LocalVoxelCoords.Z < FExtendedSDF::Resolution - 2; ++LocalVoxelCoords.Z)
		{
			for (LocalVoxelCoords.Y = -2; LocalVoxelCoords.Y < FExtendedSDF::Resolution - 2; ++LocalVoxelCoords.Y)
			{
				for (LocalVoxelCoords.X = -2; LocalVoxelCoords.X < FExtendedSDF::Resolution - 2; ++LocalVoxelCoords.X, ++SDFIndex)
				{
					check(SDFIndex >= 0 && SDFIndex < ExtendedSDF->SDF.Num());
					const FIntVector GlobalCoords = BaseGlobalCoords + LocalVoxelCoords;
					const FVoxelAddress VoxelAddress = Grid.CalcVoxelAddressFromGlobalCoords(GlobalCoords);
					const FVoxel* Voxel = Grid.ResolveAddress(VoxelAddress);
					ExtendedSDF->SDF[SDFIndex] = Voxel ? (Voxel->bSolid ? -1.0f : 1.0f) : NAN;
				}
			}
		}
		return ExtendedSDF;
	}

	/**
	 * Separable 3D gaussian kernel calculated using python:
	 *	def gaussian(x):
	 *		return math.exp(-(x*x))
	 *	kernel = [gaussian(-1), gaussian(0), gaussian(1)]
	 *	kernel_sum = sum(kernel)
	 *	kernel_norm = [x / kernel_sum for x in kernel]
	 */
	struct FGaussianKernel3D
	{
		static constexpr float WeightsX[3] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
		static constexpr float WeightsY[3] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
		static constexpr float WeightsZ[3] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
	};

	template <typename T>
	T&& ForwardUnlessNaN(T&& Value, T&& Or = T{})
	{
		return FMath::IsNaN(Value) ? static_cast<T&&>(Or) : static_cast<T&&>(Value);
	}

	template <typename FSeparableKernel>
	void ConvolveSDF(TUniquePtr<FExtendedSDF>& ExtendedSDF)
	{
		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_ConvolveSDF, FColorList::NeonBlue)

		TUniquePtr<FExtendedSDF> ConvolvedSDF = MakeUnique<FExtendedSDF>();
		ConvolvedSDF->Reset();

		auto Clamp = [](int32 Coord) { return FMath::Clamp(Coord, 0, FExtendedSDF::Resolution - 1); };

		// X pass
		ExtendedSDF->Iterate([&](float Value, int32 Index, const FIntVector& Coords)
		{
			if (FMath::IsNaN(Value))
			{
				ConvolvedSDF->SDF[Index] = NAN;
				return;
			}

			float ConvolvedValue = 0;
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsX[0] * ExtendedSDF->Get({ Clamp(Coords.X - 1), Coords.Y, Coords.Z }));
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsX[1] * ExtendedSDF->SDF[Index]);
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsX[2] * ExtendedSDF->Get({ Clamp(Coords.X + 1), Coords.Y, Coords.Z }));
			ConvolvedSDF->SDF[Index] = ConvolvedValue;
		});

		Swap(ConvolvedSDF, ExtendedSDF);

		// Y pass
		ExtendedSDF->Iterate([&](float Value, int32 Index, const FIntVector& Coords)
		{
			if (FMath::IsNaN(Value))
			{
				ConvolvedSDF->SDF[Index] = NAN;
				return;
			}

			float ConvolvedValue = 0;
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsY[0] * ExtendedSDF->Get({ Coords.X, Clamp(Coords.Y - 1), Coords.Z }));
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsY[1] * ExtendedSDF->SDF[Index]);
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsY[2] * ExtendedSDF->Get({ Coords.X, Clamp(Coords.Y + 1), Coords.Z }));
			ConvolvedSDF->SDF[Index] = ConvolvedValue;
		});

		Swap(ConvolvedSDF, ExtendedSDF);

		// Z pass
		ExtendedSDF->Iterate([&](float Value, int32 Index, const FIntVector& Coords)
		{
			if (FMath::IsNaN(Value))
			{
				ConvolvedSDF->SDF[Index] = NAN;
				return;
			}

			float ConvolvedValue = 0;
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsZ[0] * ExtendedSDF->Get({ Coords.X, Coords.Y, Clamp(Coords.Z - 1) }));
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsZ[1] * ExtendedSDF->SDF[Index]);
			ConvolvedValue += ForwardUnlessNaN(FSeparableKernel::WeightsZ[2] * ExtendedSDF->Get({ Coords.X, Coords.Y, Clamp(Coords.Z + 1) }));
			ConvolvedSDF->SDF[Index] = ConvolvedValue;
		});

		ExtendedSDF = MoveTemp(ConvolvedSDF);
	}

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

		FVector AveragePosition = FVector::ZeroVector;
		int32 IntersectingEdges = 0;
		for (int32 Edge = 0; Edge < 12; ++Edge)
		{
			const ECorner (&EdgeCorners)[2] = CellEdges[Edge];
			if (IsCellEdgeIntersectingSurface(Cell, EdgeCorners[0], EdgeCorners[1], nullptr))
			{
				const float Alpha = -Cell.Values[EdgeCorners[0]] / (Cell.Values[EdgeCorners[1]] - Cell.Values[EdgeCorners[0]]);
				AveragePosition += FMath::Lerp(Cell.Positions[EdgeCorners[0]], Cell.Positions[EdgeCorners[1]], Alpha);
				++IntersectingEdges;
			}
		}
		AveragePosition /= static_cast<FVector::FReal>(IntersectingEdges);
		return AveragePosition;
	}

	inline bool MakeCellAtVoxel(
		const FVoxelChunk::FTransformData& ChunkTransformData,
		const FIntVector& VoxelCoords,
		const FExtendedSDF& ExtendedSDF,
		FCell& OutCell)
	{
		for (int32 CI = 0; CI < 8; ++CI)
		{
			FIntVector CornerCoords;
			CornerCoords.X = VoxelCoords.X + !!(CI & 1 << 0);
			CornerCoords.Y = VoxelCoords.Y + !!(CI & 1 << 1);
			CornerCoords.Z = VoxelCoords.Z + !!(CI & 1 << 2);

			const float Value = ExtendedSDF.GetByVoxelCoords(CornerCoords);
			if (FMath::IsNaN(Value))
			{
				return false;
			}

			OutCell.Values[CI] = Value;
			OutCell.Positions[CI] = FVoxelChunk::CoordsToWorld_Static(CornerCoords, ChunkTransformData);
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

		TUniquePtr<FExtendedSDF> ExtendedSDF = GenerateChunkExtendedSDF(VoxelGrid, ChunkIndex);
		ConvolveSDF<FGaussianKernel3D>(ExtendedSDF);

		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_TriangulateVoxelChunk_MakeCells, FColorList::GreenYellow)

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
						if (!MakeCellAtVoxel(TransformData, VoxelCoords, *ExtendedSDF, Cell))
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
		}

		TArray<Debug::FSurfacePointVis> SurfacePointVises;

		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_TriangulateVoxelChunk_MakeQuads, FColorList::Scarlet)

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
