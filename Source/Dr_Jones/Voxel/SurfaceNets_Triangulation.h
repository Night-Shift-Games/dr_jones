// Property of Night Shift Games, all rights reserved.

#pragma once

#include "IndexTypes.h"
#include "VoxelEngine.h"

namespace NS::SurfaceNets
{
	using namespace NSVE;
	using namespace UE::Geometry;

	struct FSurfaceNetsChunkField;

	inline TAutoConsoleVariable CVar_Convolution(
		TEXT("NS.VE.SurfaceNets.Convolution"),
		true,
		TEXT("Voxel Engine Surface Nets - enable SDF convolution before triangulation."),
		ECVF_Cheat);

	inline TAutoConsoleVariable CVar_EdgeAverage(
		TEXT("NS.VE.SurfaceNets.EdgeAverage"),
		true,
		TEXT("Voxel Engine Surface Nets - enable surface point position averaging using edge intersections with the iso-surface."),
		ECVF_Cheat);

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

		inline TAutoConsoleVariable CVar_SurfaceNetsField(
			TEXT("NS.VE.SurfaceNets.Debug.Field"),
			false,
			TEXT("Voxel Engine Surface Nets - enable surface nets chunk field debug visualization."),
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
			TArray<TSharedPtr<FSurfaceNetsChunkField>> SurfaceNetsFields;
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

	// A generic 3D field
	template <typename T, int32 Res>
	struct alignas(Alignment) TField3D
	{
		using FElement = T;
		static constexpr int32 Resolution = Res;
		static_assert(Resolution > 0);

		static constexpr int32 NumElements = Resolution * Resolution * Resolution;
		TStaticArray<FElement, NumElements> Array;

		FElement& Get(int32 Index)
		{
			check(Index >= 0 && Index < NumElements);
			return Array[Index];
		}

		const FElement& Get(int32 Index) const
		{
			return const_cast<TField3D*>(this)->Get(Index);
		}

		FElement& Get(const FIntVector& Coords)
		{
			check(Utils::IsInlineGridPositionValid(Coords, Resolution));
			return Array[Utils::InlineGridPositionToIndex(Coords, Resolution)];
		}

		const FElement& Get(const FIntVector& Coords) const
		{
			return const_cast<TField3D*>(this)->Get(Coords);
		}

		// FFunc - void(FElement, int32, FIntVector)
		template <typename FFunc>
		void Iterate(FFunc ForEach)
		{
			SCOPED_NAMED_EVENT(NSVE_Field3D_Iterate, FColorList::MediumGoldenrod)

			int32 Index = 0;
			FIntVector Coords;
			for (Coords.Z = 0; Coords.Z < Resolution; ++Coords.Z)
			{
				for (Coords.Y = 0; Coords.Y < Resolution; ++Coords.Y)
				{
					for (Coords.X = 0; Coords.X < Resolution; ++Coords.X, ++Index)
					{
						ForEach(Array[Index], Index, Coords);
					}
				}
			}
		}

		// FFunc - void(FElement, int32, FIntVector)
		template <typename FFunc>
		void Iterate(FFunc ForEach) const
		{
			return const_cast<TField3D*>(this)->Iterate(ForEach);
		}

		// FFunc - void(int32, FIntVector)
		template <typename FFunc>
		static void Iterate_Static(FFunc ForEach)
		{
			SCOPED_NAMED_EVENT(NSVE_Field3D_Iterate, FColorList::MediumGoldenrod)

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

		void Fill(const FElement& Value)
		{
			SCOPED_NAMED_EVENT(NSVE_Field3D_Fill, FColorList::DarkOrchid)

			for (int32 Index = 0; Index < NumElements; ++Index)
			{
				Array[Index] = Value;
			}
		}

		FORCEINLINE static constexpr int32 Num() { return NumElements; }

		FORCEINLINE FElement& operator[](size_t Index) { return Array[Index]; }
		FORCEINLINE const FElement& operator[](size_t Index) const { return Array[Index]; }
	};

	// Two samples at the end of each axis are needed for the quad generation.
	// Two additional samples on each side are needed for convolution.
	static constexpr int32 ChunkFieldResolution = FVoxelChunk::Resolution + 6;

	// A field that allows for triangulating the whole chunk using surface nets without querying the voxel grid.
	struct alignas(Alignment) FSurfaceNetsChunkField : TField3D<float, ChunkFieldResolution>
	{
		static constexpr float NoneValue = NAN;
		static constexpr float ZeroValue = 0.0f;

		static bool IsValueNone(float Value)
		{
			return FMath::IsNaN(Value);
		}
	};

	// Used as an element in the material weight field
	struct FMaterialWeights
	{
		static constexpr int32 Num = FVoxel::MaterialIDNum;
		float Weights[Num];

		// ConvolveField interface:

		FORCEINLINE friend FMaterialWeights operator*(float Value, const FMaterialWeights& Weights)
		{
			// return Weights;
			FMaterialWeights MaterialWeights;
			for (int32 I = 0; I < Num; ++I)
			{
				MaterialWeights.Weights[I] = Value * Weights.Weights[I];
			}
			return MaterialWeights;
		}

		FORCEINLINE FMaterialWeights& operator+=(const FMaterialWeights& Other)
		{
			for (int32 I = 0; I < Num; ++I)
			{
				Weights[I] += Other.Weights[I];
			}
			return *this;
		}

		// ConvolveField interface End

		FORCEINLINE static FMaterialWeights MakeNoneMaterialWeights()
		{
			FMaterialWeights MaterialWeights;
			for (uint32 I = 0; I < Num; ++I)
			{
				MaterialWeights.Weights[I] = NAN;
			}
			return MaterialWeights;
		}

		FORCEINLINE static FMaterialWeights MakeZeroMaterialWeights()
		{
			FMaterialWeights MaterialWeights;
			for (uint32 I = 0; I < Num; ++I)
			{
				MaterialWeights.Weights[I] = 0.0f;
			}
			return MaterialWeights;
		}
	};

	// Two additional samples on each side are needed for convolution.
	static constexpr int32 ChunkMaterialFieldResolution = FVoxelChunk::Resolution + 4;

	// Voxel material index field representation - for later convolution & better blending
	struct alignas(Alignment) FChunkMaterialField : TField3D<FMaterialWeights, ChunkMaterialFieldResolution>
	{
		inline static const FMaterialWeights NoneValue = FMaterialWeights::MakeNoneMaterialWeights();
		inline static const FMaterialWeights ZeroValue = FMaterialWeights::MakeZeroMaterialWeights();

		static bool IsValueNone(const FMaterialWeights& Value)
		{
			const bool bNone = FMemory::Memcmp(&Value, &NoneValue, sizeof(FMaterialWeights)) == 0;
			return bNone;
		}
	};

	template <typename T, int32 Res, int32 ChunkOffset = 2>
	T& FieldGetByVoxelCoords(TField3D<T, Res>& Field, const FIntVector& VoxelCoords)
	{
		// Field's coords start at -ChunkOffset in relation to its chunk.
		// This is needed to make convolution by a specific kernel seamless.
		FIntVector FieldCoords;
		FieldCoords.X = VoxelCoords.X + ChunkOffset;
		FieldCoords.Y = VoxelCoords.Y + ChunkOffset;
		FieldCoords.Z = VoxelCoords.Z + ChunkOffset;
		return Field.Get(FieldCoords);
	}

	template <typename T, int32 Res, int32 ChunkOffset = 2>
	const T& FieldGetByVoxelCoords(const TField3D<T, Res>& Field, const FIntVector& VoxelCoords)
	{
		return FieldGetByVoxelCoords<T, Res, ChunkOffset>(const_cast<TField3D<T, Res>&>(Field), VoxelCoords);
	}

	struct FChunkFields
	{
		TUniquePtr<FSurfaceNetsChunkField> SurfaceNetsField;
		TUniquePtr<FChunkMaterialField> MaterialField;
	};

	inline FChunkFields GenerateChunkFields(const FVoxelGrid& Grid, int32 ChunkIndex)
	{
		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_GenerateChunkField, FColorList::NeonPink)

		FChunkFields ChunkFields;
		const FIntVector BaseGlobalCoords = Grid.CalcGlobalCoordsFromVoxelAddressChecked(MakeVoxelAddress(Grid.IndexToCoords(ChunkIndex), FIntVector{}));
		ChunkFields.SurfaceNetsField = MakeUnique<FSurfaceNetsChunkField>();
		ChunkFields.MaterialField = MakeUnique<FChunkMaterialField>();

		FIntVector LocalVoxelCoords;
		int32 SurfaceNetsFieldIndex = 0;
		int32 MaterialFieldIndex = 0;

		for (LocalVoxelCoords.Z = -2; LocalVoxelCoords.Z + 2 < FSurfaceNetsChunkField::Resolution; ++LocalVoxelCoords.Z)
		{
			for (LocalVoxelCoords.Y = -2; LocalVoxelCoords.Y + 2 < FSurfaceNetsChunkField::Resolution; ++LocalVoxelCoords.Y)
			{
				for (LocalVoxelCoords.X = -2; LocalVoxelCoords.X + 2 < FSurfaceNetsChunkField::Resolution; ++LocalVoxelCoords.X, ++SurfaceNetsFieldIndex)
				{
					check(SurfaceNetsFieldIndex >= 0 && SurfaceNetsFieldIndex < ChunkFields.SurfaceNetsField->Num());
					FIntVector GlobalCoords = BaseGlobalCoords + LocalVoxelCoords;

					const FIntVector ChunkCoords = Grid.CalcChunkCoordsFromSignedGlobalCoords(GlobalCoords);
					// TODO: Fix this shit
					if (Grid.FindChunkByCoords(ChunkCoords) == nullptr)
					{
						GlobalCoords.X = FMath::Clamp(GlobalCoords.X, BaseGlobalCoords.X, BaseGlobalCoords.X + FVoxelChunk::Resolution - 1);
						GlobalCoords.Y = FMath::Clamp(GlobalCoords.Y, BaseGlobalCoords.Y, BaseGlobalCoords.Y + FVoxelChunk::Resolution - 1);
						GlobalCoords.Z = FMath::Clamp(GlobalCoords.Z, BaseGlobalCoords.Z, BaseGlobalCoords.Z + FVoxelChunk::Resolution - 1);
					}

					FVoxelAddress VoxelAddress = Grid.CalcVoxelAddressFromGlobalCoords(GlobalCoords);
					const FVoxel* Voxel = Grid.ResolveAddress(VoxelAddress);
					ChunkFields.SurfaceNetsField->Get(SurfaceNetsFieldIndex) = Voxel ? (Voxel->bSolid ? -1.0f : 1.0f) : NAN;

					const bool bMaterialVoxel = LocalVoxelCoords.X + 2 < FChunkMaterialField::Resolution &&
						LocalVoxelCoords.Y + 2 < FChunkMaterialField::Resolution &&
						LocalVoxelCoords.Z + 2 < FChunkMaterialField::Resolution;

					if (bMaterialVoxel)
					{
						FMaterialWeights& MaterialWeights = ChunkFields.MaterialField->Get(MaterialFieldIndex);
						for (int32 MI = 0; MI < FMaterialWeights::Num; ++MI)
						{
							MaterialWeights.Weights[MI] = Voxel ? (Voxel->LocalMaterial == MI ? 1.0f : 0.0f) : 0.0f;
						}
						++MaterialFieldIndex;
					}
				}
			}
		}

		return ChunkFields;
	}

	/**
	 * Separable 3D gaussian kernel calculated using python:
	 *
	 *	def gaussian(x):
	 *		return math.exp(-(x*x))
	 *
	 *	def make_kernel():
	 *		kernel = [gaussian(-1), gaussian(0), gaussian(1)]
	 *		kernel_sum = sum(kernel)
	 *		kernel_norm = [x / kernel_sum for x in kernel]
	 *		return kernel_norm
	 */
	struct FGaussianKernel3D_3x3x3
	{
		static constexpr int32 Size = 3;
		static constexpr float WeightsX[Size] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
		static constexpr float WeightsY[Size] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
		static constexpr float WeightsZ[Size] = { 0.21194155761708544f, 0.5761168847658291f, 0.21194155761708544f };
	};

	/**
	 * Same as above, but with different coefficients:
	 *
	 *	def gaussian(x):
	 *		return math.exp(-(x*x)/2.4)
	 *
	 *	def make_kernel():
	 *		kernel = [gaussian(-2), gaussian(-1), gaussian(0), gaussian(1), gaussian(2)]
	 *		kernel_sum = sum(kernel)
	 *		kernel_norm = [x / kernel_sum for x in kernel]
	 *		return kernel_norm
	 */
	struct FGaussianKernel3D_5x5x5
	{
		static constexpr int32 Size = 5;
		static constexpr float WeightsX[Size] = { 0.0700516758899665f, 0.24450437360094404f, 0.37088790101817887f, 0.24450437360094404f, 0.0700516758899665f };
		static constexpr float WeightsY[Size] = { 0.0700516758899665f, 0.24450437360094404f, 0.37088790101817887f, 0.24450437360094404f, 0.0700516758899665f };
		static constexpr float WeightsZ[Size] = { 0.0700516758899665f, 0.24450437360094404f, 0.37088790101817887f, 0.24450437360094404f, 0.0700516758899665f };
	};

	template <typename FField, typename T>
	T&& ForwardUnlessNone(T&& Value, T&& Or = T{})
	{
		const bool bIsNone = FField::IsValueNone(Value);
		return bIsNone ? static_cast<T&&>(Or) : static_cast<T&&>(Value);
	}

	template <typename FSeparableKernel, typename FField>
	void ConvolveField(TUniquePtr<FField>& Field)
	{
		static constexpr int32 KernelSize = FSeparableKernel::Size;
		static_assert(KernelSize >= 3 && KernelSize % 2 == 1);
		static constexpr int32 KernelEnd = (KernelSize - 1) / 2;
		static constexpr int32 KernelStart = -KernelEnd;

		using FElement = typename FField::FElement;

		SCOPED_NAMED_EVENT(NSVE_SurfaceNets_ConvolveField, FColorList::NeonBlue)

		TUniquePtr<FField> ConvolvedField = MakeUnique<FField>();
		ConvolvedField->Fill(FField::NoneValue);

		auto Clamp = [](int32 Coord) { return FMath::Clamp(Coord, 0, FField::Resolution - 1); };

		// X pass
		Field->Iterate([&](const FElement& Value, int32 Index, const FIntVector& Coords)
		{
			if (FField::IsValueNone(Value))
			{
				ConvolvedField->Get(Index) = FField::NoneValue;
				return;
			}

			FElement& ConvolvedValue = ConvolvedField->Get(Index);
			ConvolvedValue = FField::ZeroValue;

			int32 WeightIndex = 0;
			for (int32 Offset = KernelStart; Offset <= KernelEnd; ++Offset, ++WeightIndex)
			{
				ConvolvedValue += ForwardUnlessNone<FField>(FSeparableKernel::WeightsX[WeightIndex] * Field->Get({ Clamp(Coords.X + Offset), Coords.Y, Coords.Z }));
			}
		});

		Swap(ConvolvedField, Field);

		// Y pass
		Field->Iterate([&](const FElement& Value, int32 Index, const FIntVector& Coords)
		{
			if (FField::IsValueNone(Value))
			{
				ConvolvedField->Get(Index) = FField::NoneValue;
				return;
			}

			FElement& ConvolvedValue = ConvolvedField->Get(Index);
			ConvolvedValue = FField::ZeroValue;

			int32 WeightIndex = 0;
			for (int32 Offset = KernelStart; Offset <= KernelEnd; ++Offset, ++WeightIndex)
			{
				ConvolvedValue += ForwardUnlessNone<FField>(FSeparableKernel::WeightsY[WeightIndex] * Field->Get({ Coords.X, Clamp(Coords.Y + Offset), Coords.Z }));
			}
		});

		Swap(ConvolvedField, Field);

		// Z pass
		Field->Iterate([&](const FElement& Value, int32 Index, const FIntVector& Coords)
		{
			if (FField::IsValueNone(Value))
			{
				ConvolvedField->Get(Index) = FField::NoneValue;
				return;
			}

			FElement& ConvolvedValue = ConvolvedField->Get(Index);
			ConvolvedValue = FField::ZeroValue;

			int32 WeightIndex = 0;
			for (int32 Offset = KernelStart; Offset <= KernelEnd; ++Offset, ++WeightIndex)
			{
				ConvolvedValue += ForwardUnlessNone<FField>(FSeparableKernel::WeightsZ[WeightIndex] * Field->Get({ Coords.X, Coords.Y, Clamp(Coords.Z + Offset) }));
			}
		});

		Field = MoveTemp(ConvolvedField);
	}

	inline bool IsCellEdgeIntersectingSurface(const FCell& Cell, ECorner CornerA, ECorner CornerB, bool& bOutCornerBInside)
	{
		check(CornerA < 8 && CornerB < 8);
		const bool bIntersects = Cell.Values[CornerA] > 0.0f != Cell.Values[CornerB] > 0.0f;
		if (bIntersects)
		{
			bOutCornerBInside = Cell.Values[CornerB] <= 0.0f;
		}
		return bIntersects;
	}

	inline bool IsCellEdgeIntersectingSurface(const FCell& Cell, ECorner CornerA, ECorner CornerB)
	{
		check(CornerA < 8 && CornerB < 8);
		return Cell.Values[CornerA] > 0.0f != Cell.Values[CornerB] > 0.0f;
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

		if (LIKELY(CVar_EdgeAverage.GetValueOnAnyThread()))
		{
			FVector AveragePosition = FVector::ZeroVector;
			int32 IntersectingEdges = 0;
			for (int32 Edge = 0; Edge < 12; ++Edge)
			{
				const ECorner (&EdgeCorners)[2] = CellEdges[Edge];
				if (IsCellEdgeIntersectingSurface(Cell, EdgeCorners[0], EdgeCorners[1]))
				{
					const float Alpha = -Cell.Values[EdgeCorners[0]] / (Cell.Values[EdgeCorners[1]] - Cell.Values[EdgeCorners[0]]);
					AveragePosition += FMath::Lerp(Cell.Positions[EdgeCorners[0]], Cell.Positions[EdgeCorners[1]], Alpha);
					++IntersectingEdges;
				}
			}
			AveragePosition /= static_cast<FVector::FReal>(IntersectingEdges);
			return AveragePosition;
		}
		else
		{
			FVector AveragePosition = FVector::ZeroVector;
			for (int32 CI = 0; CI < 8; ++CI)
			{
				AveragePosition += Cell.Positions[CI];
			}
			AveragePosition /= 8.0;
			return AveragePosition;
		}
	}

	inline bool MakeCellAtVoxel(
		const FVoxelChunk::FTransformData& ChunkTransformData,
		const FIntVector& VoxelCoords,
		const FSurfaceNetsChunkField& ChunkField,
		FCell& OutCell)
	{
		for (int32 CI = 0; CI < 8; ++CI)
		{
			FIntVector CornerCoords;
			CornerCoords.X = VoxelCoords.X + !!(CI & 1 << 0);
			CornerCoords.Y = VoxelCoords.Y + !!(CI & 1 << 1);
			CornerCoords.Z = VoxelCoords.Z + !!(CI & 1 << 2);

			const float Value = FieldGetByVoxelCoords(ChunkField, CornerCoords);
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
		TArray<FMaterialWeights>& OutMaterialWeights,
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
		const FIntVector ChunkCoords = VoxelGrid.IndexToCoords(ChunkIndex);
		constexpr int32 CellCount = (FVoxelChunk::Resolution + 2) * (FVoxelChunk::Resolution + 2) * (FVoxelChunk::Resolution + 2);

		TArray<FCell> Cells;
		TArray<FSurfacePoint> SurfacePoints;
		TMap<FIntVector, int32> CoordsToSurfacePointIndexMap;

		Cells.SetNumZeroed(CellCount);
		SurfacePoints.Reserve(CellCount);
		CoordsToSurfacePointIndexMap.Reserve(CellCount);

		FChunkFields ChunkFields = GenerateChunkFields(VoxelGrid, ChunkIndex);
		if (LIKELY(CVar_Convolution.GetValueOnAnyThread()))
		{
			ConvolveField<FGaussianKernel3D_3x3x3>(ChunkFields.SurfaceNetsField);
			ConvolveField<FGaussianKernel3D_5x5x5>(ChunkFields.MaterialField);
		}

		if (DebugContext)
		{
			check(ChunkFields.SurfaceNetsField.IsValid());
			DebugContext->SurfaceNetsFields.SetNum(FMath::Max(DebugContext->SurfaceNetsFields.Num(), ChunkIndex + 1));
			DebugContext->SurfaceNetsFields[ChunkIndex] = MakeShared<FSurfaceNetsChunkField>();
			*DebugContext->SurfaceNetsFields[ChunkIndex].Get() = *ChunkFields.SurfaceNetsField.Get();
		}

		{
			SCOPED_NAMED_EVENT(NSVE_SurfaceNets_TriangulateVoxelChunk_MakeCells, FColorList::GreenYellow)

			int32 CellIndex = 0;
			FIntVector VoxelCoords;
			// NOTE: One more cell per dimension is needed for creating quads at chunk's borders
			for (VoxelCoords.Z = -1; VoxelCoords.Z < FVoxelChunk::Resolution + 1; ++VoxelCoords.Z)
			{
				for (VoxelCoords.Y = -1; VoxelCoords.Y < FVoxelChunk::Resolution + 1; ++VoxelCoords.Y)
				{
					for (VoxelCoords.X = -1; VoxelCoords.X < FVoxelChunk::Resolution + 1; ++VoxelCoords.X, ++CellIndex)
					{
						if (VoxelCoords.GetMin() == -1)
						{
							const FIntVector GlobalCoords = VoxelGrid.CalcGlobalCoordsFromVoxelAddress(MakeVoxelAddress(ChunkCoords, VoxelCoords));
							const FVoxelAddress VoxelAddress = VoxelGrid.CalcVoxelAddressFromSignedGlobalCoords(GlobalCoords);
							// Means the chunk does exists, therefore it will generate border quads
							if (VoxelGrid.FindChunkByCoords(VoxelAddress.ChunkCoords) != nullptr)
							{
								continue;
							}
						}

						FCell& Cell = Cells[CellIndex];
						if (!MakeCellAtVoxel(TransformData, VoxelCoords, *ChunkFields.SurfaceNetsField, Cell))
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

						OutVertices.Add(SurfacePointRef.Location);

						const FMaterialWeights& MaterialWeights = FieldGetByVoxelCoords(*ChunkFields.MaterialField, VoxelCoords);
						OutMaterialWeights.Add(MaterialWeights);
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
				check(CellCoords.X >= -1 && CellCoords.X < FVoxelChunk::Resolution + 2);
				check(CellCoords.Y >= -1 && CellCoords.Y < FVoxelChunk::Resolution + 2);
				check(CellCoords.Z >= -1 && CellCoords.Z < FVoxelChunk::Resolution + 2);
				const int32 SurfacePointIndex = It.GetIndex();

				FCell& Cell = Cells[SurfacePoint.CellIndex];

				bool bXYZCornerInside;
				uint8 EdgeIntersections = 0;

				// YZ Edge - X Axis
				if (IsCellEdgeIntersectingSurface(Cell, Corner_YZ, Corner_XYZ, bXYZCornerInside))
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
				if (IsCellEdgeIntersectingSurface(Cell, Corner_XZ, Corner_XYZ, bXYZCornerInside))
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
				if (IsCellEdgeIntersectingSurface(Cell, Corner_XY, Corner_XYZ, bXYZCornerInside))
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
		TArray<FMaterialWeights>& OutMaterialWeights,
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
			TArray<FMaterialWeights> MaterialWeights;
			TriangulateVoxelChunk(VoxelGrid, Chunk, ChunkIndex, Vertices, Triangles, MaterialWeights, DebugContext);

			{
				FScopeLock Lock(&TransactionGuard);

				Algo::Copy(Vertices, OutVertices);
				Algo::Copy(MaterialWeights, OutMaterialWeights);
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
