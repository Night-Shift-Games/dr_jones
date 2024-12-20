﻿// Property of Night Shift Games, all rights reserved.

#pragma once

#define ENABLE_VOXEL_ENGINE_DEBUG (ENABLE_VISUAL_LOG)

DECLARE_STATS_GROUP(TEXT("VoxelEngine"), STATGROUP_VoxelEngine, STATCAT_Advanced);

namespace NSVE
{
	// Global compile-time data used by the voxel engine
	struct FVoxelEngineConfig
	{
		// Size of the chunk in UU. Every chunk is a cube
		static constexpr double ChunkSize = 200.0;
		static constexpr double HalfChunkSize = ChunkSize / 2.0;

		// How many voxels are in each chunk (in one dimension)
		static constexpr int32 ChunkResolution = 1 << 4;

		// Whether to support multi-threaded parallel iteration of chunks (FVoxelGrid::IterateChunks_Parallel)
		static constexpr bool bEnableParallelChunkIteration = true;
	};

	static_assert(FMath::IsPowerOfTwo(FVoxelEngineConfig::ChunkResolution));

	inline TAutoConsoleVariable CVar_ParallelChunkIteration(
		TEXT("NS.VE.ParallelChunkIteration"),
		true,
		TEXT("Voxel Engine - enable parallel chunk iteration (using parallel-for)."),
		ECVF_Default);

	// Utility functions used in the voxel engine
	namespace Utils
	{
		[[nodiscard]] FORCEINLINE constexpr FIntVector IndexToInlineGridPosition(int32 Index, const FIntVector& GridDimensions)
		{
			check(Index >= 0 && Index < GridDimensions.X * GridDimensions.Y * GridDimensions.Z);

			FIntVector Position;
			Position.X = Index % GridDimensions.X;
			Position.Y = Index / GridDimensions.X % GridDimensions.Y;
			Position.Z = Index / (GridDimensions.X * GridDimensions.Y) % GridDimensions.Z;
			return Position;
		}

		[[nodiscard]] FORCEINLINE constexpr FIntVector IndexToInlineGridPosition(int32 Index, int32 UniformGridSize)
		{
			check(Index >= 0 && Index < UniformGridSize * UniformGridSize * UniformGridSize);

			FIntVector Position;
			Position.X = Index % UniformGridSize;
			Position.Y = Index / UniformGridSize % UniformGridSize;
			Position.Z = Index / (UniformGridSize * UniformGridSize) % UniformGridSize;
			return Position;
		}

		[[nodiscard]] FORCEINLINE constexpr int32 InlineGridPositionToIndex(const FIntVector& Position, const FIntVector& GridDimensions)
		{
			check(Position.X >= 0 && Position.X < GridDimensions.X);
			check(Position.Y >= 0 && Position.Y < GridDimensions.Y);
			check(Position.Z >= 0 && Position.Z < GridDimensions.Z);

			int32 Index = Position.X;
			Index += Position.Y * GridDimensions.X;
			Index += Position.Z * GridDimensions.X * GridDimensions.Y;
			return Index;
		}

		[[nodiscard]] FORCEINLINE constexpr int32 InlineGridPositionToIndex(const FIntVector& Position, int32 UniformGridSize)
		{
			check(Position.X >= 0 && Position.X < UniformGridSize);
			check(Position.Y >= 0 && Position.Y < UniformGridSize);
			check(Position.Z >= 0 && Position.Z < UniformGridSize);

			int32 Index = Position.X;
			Index += Position.Y * UniformGridSize;
			Index += Position.Z * UniformGridSize * UniformGridSize;
			return Index;
		}

		[[nodiscard]] FORCEINLINE constexpr bool IsInlineGridPositionValid(const FIntVector& Position, const FIntVector& GridDimensions)
		{
			return Position.X >= 0 && Position.X < GridDimensions.X &&
				Position.Y >= 0 && Position.Y < GridDimensions.Y &&
				Position.Z >= 0 && Position.Z < GridDimensions.Z;
		}

		[[nodiscard]] FORCEINLINE constexpr bool IsInlineGridPositionValid(const FIntVector& Position, int32 UniformGridSize)
		{
			return Position.X >= 0 && Position.X < UniformGridSize &&
				Position.Y >= 0 && Position.Y < UniformGridSize &&
				Position.Z >= 0 && Position.Z < UniformGridSize;
		}
	};

	// Raw packed data of a single voxel
	struct FVoxel
	{
		static constexpr int32 BitsForMaterialID = 3;
		static constexpr int32 MaterialIDNum = 2 << (BitsForMaterialID - 1);
		union
		{
			struct
			{
				uint8 bSolid : 1;
				uint8 LocalMaterial : BitsForMaterialID; // 8 materials per chunk (or maybe even per level) should be enough
			};
			uint8 Data;
		};
	};

	inline constexpr size_t Alignment = std::hardware_constructive_interference_size;

	// Inline array of voxels with utility operations
	class DR_JONES_API alignas(Alignment) FVoxelArray
	{
	public:
		using FElement = FVoxel;

		static constexpr size_t ElementSize = sizeof(FElement);

		static constexpr int32 Resolution = FVoxelEngineConfig::ChunkResolution;

		// Max count of voxels in a single voxel array
		static constexpr int32 ElementCount = Resolution * Resolution * Resolution;

		// Max size of raw allocated data of a single voxel array
		static constexpr int32 MaxSizeInBytes = ElementCount * ElementSize;

		static inline const FIntVector Dimensions = FIntVector(Resolution);

		TStaticArray<FElement, ElementCount> Voxels;

		void Clear();
		void Fill(FVoxel Voxel);

		void FillTestCube(const FIntVector& Offset = {});

		// If it's either completely solid, or empty
		bool IsUniformlySolid() const;

		FVoxel& Get(int32 Index);
		FVoxel& GetAtCoords(const FIntVector3& Coords);

		const FVoxel& Get(int32 Index) const;
		const FVoxel& GetAtCoords(const FIntVector3& Coords) const;

		// Iterate through all voxels using a function with signature void(FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		template <typename Func>
		void Iterate(Func IterateFunc)
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IterateVoxels, FColorList::MandarianOrange)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IterateVoxels"), STAT_VoxelEngine_FVoxelArray_IterateVoxels, STATGROUP_VoxelEngine)

			FIntVector Coords;
			int32 Index = 0;
			for (Coords.Z = 0; Coords.Z < Resolution; ++Coords.Z)
			{
				for (Coords.Y = 0; Coords.Y < Resolution; ++Coords.Y)
				{
					for (Coords.X = 0; Coords.X < Resolution; ++Coords.X, ++Index)
					{
						check(IsValidIndex(Index));
						FElement& Voxel = Voxels[Index];
						IterateFunc(Voxel, Index, Coords);
					}
				}
			}
		}

		// Iterate through all voxels using a function with signature void(const FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		template <typename Func>
		void Iterate(Func IterateFunc) const
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IterateVoxels, FColorList::MandarianOrange)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IterateVoxels"), STAT_VoxelEngine_FVoxelArray_IterateVoxels, STATGROUP_VoxelEngine)

			FIntVector Coords;
			int32 Index = 0;
			for (Coords.Z = 0; Coords.Z < Resolution; ++Coords.Z)
			{
				for (Coords.Y = 0; Coords.Y < Resolution; ++Coords.Y)
				{
					for (Coords.X = 0; Coords.X < Resolution; ++Coords.X, ++Index)
					{
						check(IsValidIndex(Index));
						const FElement& Voxel = Voxels[Index];
						IterateFunc(Voxel, Index, Coords);
					}
				}
			}
		}

		// Iterate through all voxels using a function with signature void(FVoxel& Voxel, int32 Index)
		// Version without coords
		template <typename Func>
		void IterateFast(Func IterateFunc)
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IterateVoxelsFast, FColorList::Copper)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IterateVoxelsFast"), STAT_VoxelEngine_FVoxelArray_IterateVoxelsFast, STATGROUP_VoxelEngine)

			for (int32 Index = 0; Index < ElementCount; ++Index)
			{
				check(IsValidIndex(Index));
				FElement& Voxel = Voxels[Index];
				IterateFunc(Voxel, Index);
			}
		}

		// Iterate through all voxels using a function with signature void(const FVoxel& Voxel, int32 Index)
		// Version without coords
		template <typename Func>
		void IterateFast(Func IterateFunc) const
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IterateVoxelsFast, FColorList::Copper)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IterateVoxelsFast"), STAT_VoxelEngine_FVoxelArray_IterateVoxelsFast, STATGROUP_VoxelEngine)

			for (int32 Index = 0; Index < ElementCount; ++Index)
			{
				check(IsValidIndex(Index));
				const FElement& Voxel = Voxels[Index];
				IterateFunc(Voxel, Index);
			}
		}

		// Perform a boolean operation on two voxel arrays
		template <typename OpFunc>
		FVoxelArray& Op(const FVoxelArray& OtherArray, OpFunc Operation)
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Op, FColorList::YellowGreen)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Op"), STAT_VoxelEngine_FVoxelArray_Op, STATGROUP_VoxelEngine)

			check(Voxels.Num() == OtherArray.Voxels.Num()); // This will literally NEVER go off, but whatever

			for (int32 Index = 0; Index < ElementCount; ++Index)
			{
				FVoxel& Voxel = Voxels[Index];
				const FVoxel& OtherVoxel = OtherArray.Voxels[Index];
				Operation(Voxel, OtherVoxel);
			}

			return *this;
		}

		FVoxelArray& Union(const FVoxelArray& OtherArray);
		FVoxelArray& Difference(const FVoxelArray& OtherArray);
		FVoxelArray& Intersect(const FVoxelArray& OtherArray);

		static const FIntVector3& GetDimensions() { return Dimensions; }

		static constexpr bool IsValidIndex(int32 Index);

		static constexpr FIntVector IndexToCoords(int32 Index);
		static constexpr int32 CoordsToIndex(const FIntVector& Coords);

		FElement& operator[](size_t Index);
		const FElement& operator[](size_t Index) const;
	};

	// World-space bounding box of a voxel chunk
	struct FVoxelChunkBounds
	{
		FVector Origin;
		FVector::FReal Extent;

		FBox GetBox() const;
	};

	// Voxel reference for storing in an octree
	struct FVoxelRef
	{
		size_t Index;
		FBoxSphereBounds Bounds;
	};

	// Octree semantics for FVoxelRef type
	struct FVoxelSemantics
	{
		static constexpr size_t MaxElementsPerLeaf = 1;
		static constexpr size_t MinInclusiveElementsPerNode = 1;
		static constexpr size_t MaxNodeDepth = 5;

		using ElementAllocator = TInlineAllocator<MaxElementsPerLeaf>;

		static const FBoxSphereBounds& GetBoundingBox(const FVoxelRef& InVoxel);
		static bool AreElementsEqual(const FVoxelRef& A, const FVoxelRef& B);
		static void SetElementId(const FVoxelRef& Element, FOctreeElementId2 Id);
	};

	struct FVoxelLayer
	{
		float PlaneMaxZ = 0;
		uint8 LocalMaterialIndex = 0;
	};

	// Single voxel chunk that exists in a voxel grid
	// Used to store data in segments that can be loaded/unloaded on demand only when needed
	class DR_JONES_API alignas(Alignment) FVoxelChunk
	{
	public:
		static constexpr int32 Resolution = FVoxelArray::Resolution;

		FVoxelChunkBounds Bounds;
		FVoxelArray Voxels;

	public:
		explicit FVoxelChunk(const FVoxelChunkBounds& Bounds);

		void Clear();
		void FillTest();
		void FillSurface(float SurfaceZ_WS);
		void FillLayered(TArray<FVoxelLayer> VoxelLayers);

		struct FTransformData
		{
			FVector RootCenter;
			FVector::FReal RootExtent;
			FVector::FReal VoxelSize;
		};

		FTransformData MakeTransformData() const;

		FVector CoordsToWorld(const FIntVector& Coords) const;
		static FVector CoordsToWorld_Static(const FIntVector& Coords, const FTransformData& TransformData);

		FIntVector WorldToCoords(const FVector& WorldPosition) const;
		static FIntVector WorldToCoords(const FVector& WorldPosition, const FTransformData& TransformData);

		static FIntVector IndexToCoords(int32 Index);
		static int32 CoordsToIndex(const FIntVector& Coords);

		static bool AreCoordsValid(const FIntVector& Coords);

#if ENABLE_VOXEL_ENGINE_DEBUG
		void DrawDebugVoxels() const;
#endif
	};

	struct FVoxelGridInitializer
	{
		FTransform Transform;
		FBoxSphereBounds Bounds;

		// TODO: Move the fill logic outside of VoxelEngine.h
		float FillSurfaceZ_WS;
		TArray<FVoxelLayer> Layers;
		TArray<FVector> ArtifactLocations;
		TArray<double> ArtifactRadii;
	};

	// Persistent global address of a specific voxel in the *whole* voxel grid
	struct FVoxelAddress
	{
		// Coords of a chunk in the whole grid
		FIntVector ChunkCoords;
		// Local coords of a voxel in the chunk
		FIntVector VoxelCoords;
	};

	inline FVoxelAddress MakeVoxelAddress(const FIntVector& ChunkCoords, const FIntVector& VoxelCoords)
	{
		FVoxelAddress Address;
		Address.ChunkCoords = ChunkCoords;
		Address.VoxelCoords = VoxelCoords;
		return Address;
	}

	// Structure that stores voxels in a grid of separate chunks to optimize the usage
	class DR_JONES_API FVoxelGrid
	{
	public:
		FVoxelGrid();
		void Initialize(const FVoxelGridInitializer& Initializer);

		template <typename FFunc>
		void IterateChunks(FFunc ForEachChunk)
		{
			const int32 ChunkCount = DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z;
			for (int32 Index = 0; Index < ChunkCount; ++Index)
			{
				check(Chunks.IsValidIndex(Index));
				ForEachChunk(Chunks[Index], Index);
			}
		}

		template <typename FFunc>
		void IterateChunks_Parallel(FFunc ForEachChunk)
		{
			if constexpr (FVoxelEngineConfig::bEnableParallelChunkIteration)
			{
				FVoxelChunk* ChunksData = Chunks.GetData();
				const int32 ChunkCount = DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z;
				const bool bParallel = CVar_ParallelChunkIteration.GetValueOnAnyThread();
				const EParallelForFlags Flags = bParallel ? EParallelForFlags::Unbalanced : EParallelForFlags::ForceSingleThread;
				ParallelForTemplate(ChunkCount, [this, ForEachChunk, ChunksData](int32 Index)
				{
					ForEachChunk(ChunksData[Index], Index);
				}, Flags);
			}
			else
			{
				IterateChunks(ForEachChunk);
			}
		}

		template <typename FFunc>
		void IterateChunks(FFunc ForEachChunk) const
		{
			const_cast<FVoxelGrid*>(this)->IterateChunks(ForEachChunk);
		}

		template <typename FFunc>
		void IterateChunks_Parallel(FFunc ForEachChunk) const
		{
			const_cast<FVoxelGrid*>(this)->IterateChunks_Parallel(ForEachChunk);
		}

		FVoxelChunkBounds CalcChunkWorldBoundsFromIndex(int32 Index) const;
		FVoxelChunkBounds CalcChunkWorldBoundsFromGridCoords(const FIntVector& GridCoords) const;

		int32 CalcChunkIndexFromWorldPosition(const FVector& WorldLocation) const;
		int32 CalcChunkIndexFromLocalPosition(const FVector& LocalPosition) const;
		FIntVector CalcGridCoordsFromLocalPosition(const FVector& LocalPosition) const;

		FIntVector IndexToCoords(int32 Index) const;
		int32 CoordsToIndex(const FIntVector& Coords) const;

		bool AreCoordsValid(const FIntVector& Coords) const;

		TStaticArray<int32, 26> GetChunkNeighborIndices(int32 Index) const;

		FVoxelChunk* GetChunkByIndex(int32 Index);
		const FVoxelChunk* GetChunkByIndex(int32 Index) const;
		FVoxelChunk& GetChunkRefByIndex(int32 Index);
		const FVoxelChunk& GetChunkRefByIndex(int32 Index) const;

		FVoxelChunk* FindChunkByCoords(const FIntVector& Coords);
		const FVoxelChunk* FindChunkByCoords(const FIntVector& Coords) const;

		static FVoxelAddress CalcVoxelAddressFromGlobalCoords(const FIntVector& GlobalCoords);
		static FVoxelAddress CalcVoxelAddressFromSignedGlobalCoords(const FIntVector& GlobalCoords);
		static FIntVector CalcGlobalCoordsFromVoxelAddress(const FVoxelAddress& VoxelAddress);

		FVoxelAddress CalcVoxelAddressFromGlobalCoordsChecked(const FIntVector& GlobalCoords) const;
		FIntVector CalcGlobalCoordsFromVoxelAddressChecked(const FVoxelAddress& VoxelAddress) const;

		static FIntVector CalcChunkCoordsFromGlobalCoords(const FIntVector& GlobalCoords);
		static FIntVector CalcChunkCoordsFromSignedGlobalCoords(const FIntVector& GlobalCoords);

		FVoxel* ResolveAddress(const FVoxelAddress& VoxelAddress);
		const FVoxel* ResolveAddress(const FVoxelAddress& VoxelAddress) const;

#if ENABLE_VOXEL_ENGINE_DEBUG
		void DrawDebugChunks(const UWorld& World,
			const FVector& PositionInside,
			bool bPersistentLines = false,
			float LifeTime = -1.0f,
			uint8 DepthPriority = 0,
			float Thickness = 0.0f) const;
#endif

		const FTransform& GetTransform() const { return Transform; }
		const TArray<FVoxelChunk>& GetChunks() const { return Chunks; }
		const FIntVector& GetDimensionsInChunks() const { return DimensionsInChunks; }
		FIntVector GetDimensionsInVoxels() const { return DimensionsInChunks * FVoxelChunk::Resolution; }
		const FVector& GetExtent() const { return Extent; }
		int32 GetChunkCount() const { return Chunks.Num(); }

	private:
		FTransform Transform;
		TArray<FVoxelChunk> Chunks;
		FIntVector DimensionsInChunks;
		FVector Extent;

		mutable FCriticalSection ChunksGuard;
	};
}

// =============================================================================================================== //
//                                          Inline function definitions
// =============================================================================================================== //

namespace NSVE
{
	// -------------------------- FVoxelArray ----------------------------------------------------------------------

	FORCEINLINE void FVoxelArray::FillTestCube(const FIntVector& Offset)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_FillTestCube, FColorList::CadetBlue)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::FillTestCube"), STAT_VoxelEngine_FVoxelArray_FillTestCube, STATGROUP_VoxelEngine)

		Iterate([&Offset](FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		{
			static constexpr int32 LowBound = Resolution * 1 / 3;
			static constexpr int32 HighBound = Resolution * 2 / 3;
			Voxel.bSolid =
				Coords.X - Offset.X > LowBound && Coords.X - Offset.X < HighBound &&
				Coords.Y - Offset.Y > LowBound && Coords.Y - Offset.Y < HighBound &&
				Coords.Z - Offset.Z > LowBound && Coords.Z - Offset.Z < HighBound;
		});
	}

	FORCEINLINE FVoxel& FVoxelArray::Get(int32 Index)
	{
		check(IsValidIndex(Index));
		return Voxels[Index];
	}

	FORCEINLINE FVoxel& FVoxelArray::GetAtCoords(const FIntVector3& Coords)
	{
		return Get(CoordsToIndex(Coords));
	}

	FORCEINLINE const FVoxel& FVoxelArray::Get(int32 Index) const
	{
		return const_cast<FVoxelArray*>(this)->Get(Index);
	}

	FORCEINLINE const FVoxel& FVoxelArray::GetAtCoords(const FIntVector3& Coords) const
	{
		return const_cast<FVoxelArray*>(this)->GetAtCoords(Coords);
	}

	FORCEINLINE FVoxelArray& FVoxelArray::Union(const FVoxelArray& OtherArray)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Union, FColorList::YellowGreen)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Union"), STAT_VoxelEngine_FVoxelArray_Union, STATGROUP_VoxelEngine)

		return Op(OtherArray, [](FVoxel& A, const FVoxel& B)
		{
			A = B.bSolid ? B : A;
		});
	}

	FORCEINLINE FVoxelArray& FVoxelArray::Difference(const FVoxelArray& OtherArray)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Difference, FColorList::YellowGreen)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Difference"), STAT_VoxelEngine_FVoxelArray_Difference, STATGROUP_VoxelEngine)

		return Op(OtherArray, [](FVoxel& A, const FVoxel& B)
		{
			A = B.bSolid ? FVoxel{} : A;
		});
	}

	FORCEINLINE FVoxelArray& FVoxelArray::Intersect(const FVoxelArray& OtherArray)
	{
		SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_Intersect, FColorList::YellowGreen)
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::Intersect"), STAT_VoxelEngine_FVoxelArray_Intersect, STATGROUP_VoxelEngine)

		return Op(OtherArray, [](FVoxel& A, const FVoxel& B)
		{
			A = A.bSolid && B.bSolid ? A : FVoxel{};
		});
	}

	FORCEINLINE constexpr bool FVoxelArray::IsValidIndex(int32 Index)
	{
		return Index >= 0 && Index < ElementCount;
	}

	FORCEINLINE constexpr FIntVector FVoxelArray::IndexToCoords(int32 Index)
	{
		return Utils::IndexToInlineGridPosition(Index, Resolution);
	}

	FORCEINLINE constexpr int32 FVoxelArray::CoordsToIndex(const FIntVector& Coords)
	{
		return Utils::InlineGridPositionToIndex(Coords, Resolution);
	}

	FORCEINLINE FVoxelArray::FElement& FVoxelArray::operator[](size_t Index)
	{
		return Get(Index);
	}

	FORCEINLINE const FVoxelArray::FElement& FVoxelArray::operator[](size_t Index) const
	{
		return Get(Index);
	}

	// -------------------------- FVoxelChunkBounds ----------------------------------------------------------------

	FORCEINLINE FBox FVoxelChunkBounds::GetBox() const
	{
		return FBox(Origin - Extent, Origin + Extent);
	}

	// -------------------------- FVoxelSemantics ------------------------------------------------------------------

	FORCEINLINE const FBoxSphereBounds& FVoxelSemantics::GetBoundingBox(const FVoxelRef& InVoxel)
	{
		return InVoxel.Bounds;
	}

	FORCEINLINE bool FVoxelSemantics::AreElementsEqual(const FVoxelRef& A, const FVoxelRef& B)
	{
		return A.Index == B.Index;
	}

	FORCEINLINE void FVoxelSemantics::SetElementId(const FVoxelRef& Element, FOctreeElementId2 Id)
	{
	}

	// -------------------------- FVoxelChunk ----------------------------------------------------------------------

	FORCEINLINE FVoxelChunk::FTransformData FVoxelChunk::MakeTransformData() const
	{
		FTransformData TransformData;
		TransformData.RootCenter = Bounds.Origin;
		TransformData.RootExtent = Bounds.Extent;
		TransformData.VoxelSize = (TransformData.RootExtent * 2.0) / Resolution;
		return TransformData;
	}

	FORCEINLINE FVector FVoxelChunk::CoordsToWorld(const FIntVector& Coords) const
	{
		return CoordsToWorld_Static(Coords, MakeTransformData());
	}

	FORCEINLINE FVector FVoxelChunk::CoordsToWorld_Static(const FIntVector& Coords, const FTransformData& TransformData)
	{
		const FVector WorldPositionNoOffset = FVector(Coords.X, Coords.Y, Coords.Z) * TransformData.VoxelSize;
		const FVector Offset = -FVector(TransformData.RootExtent - TransformData.VoxelSize / 2.0);
		const FVector Location = TransformData.RootCenter + WorldPositionNoOffset + Offset;
		return Location;
	}

	FORCEINLINE FIntVector FVoxelChunk::WorldToCoords(const FVector& WorldPosition) const
	{
		return WorldToCoords(WorldPosition, MakeTransformData());
	}

	FORCEINLINE FIntVector FVoxelChunk::WorldToCoords(const FVector& WorldPosition, const FTransformData& TransformData)
	{
		const FVector LocalPosition = WorldPosition - (TransformData.RootCenter - TransformData.RootExtent);
		const FIntVector GridPosition = FIntVector(LocalPosition / TransformData.VoxelSize);
		return GridPosition;
	}

	FORCEINLINE FIntVector FVoxelChunk::IndexToCoords(int32 Index)
	{
		return decltype(Voxels)::IndexToCoords(Index);
	}

	FORCEINLINE int32 FVoxelChunk::CoordsToIndex(const FIntVector& Coords)
	{
		return decltype(Voxels)::CoordsToIndex(Coords);
	}

	FORCEINLINE bool FVoxelChunk::AreCoordsValid(const FIntVector& Coords)
	{
		return Coords.X >= 0 && Coords.X < Resolution &&
			Coords.Y >= 0 && Coords.Y < Resolution &&
			Coords.Z >= 0 && Coords.Z < Resolution;
	}

	// -------------------------- FVoxelChunk ----------------------------------------------------------------------

	FORCEINLINE FVoxelChunkBounds FVoxelGrid::CalcChunkWorldBoundsFromIndex(int32 Index) const
	{
		const FIntVector LocalPosition = IndexToCoords(Index);
		return CalcChunkWorldBoundsFromGridCoords(LocalPosition);
	}

	FORCEINLINE FVoxelChunkBounds FVoxelGrid::CalcChunkWorldBoundsFromGridCoords(const FIntVector& GridCoords) const
	{
		const FVector RelativePosition = FVector(GridCoords * FVoxelEngineConfig::ChunkSize) + FVoxelEngineConfig::HalfChunkSize - Extent;
		const FVector ChunkOrigin = Transform.TransformPosition(RelativePosition);
		return FVoxelChunkBounds{ ChunkOrigin, FVoxelEngineConfig::HalfChunkSize };
	}

	FORCEINLINE FIntVector FVoxelGrid::IndexToCoords(int32 Index) const
	{
		check(Index >= 0 && Index < GetChunkCount());
		return Utils::IndexToInlineGridPosition(Index, DimensionsInChunks);
	}

	FORCEINLINE int32 FVoxelGrid::CoordsToIndex(const FIntVector& Coords) const
	{
		check(AreCoordsValid(Coords));
		return Utils::InlineGridPositionToIndex(Coords, DimensionsInChunks);
	}

	FORCEINLINE bool FVoxelGrid::AreCoordsValid(const FIntVector& Coords) const
	{
		return Coords.X >= 0 && Coords.X < DimensionsInChunks.X &&
			Coords.Y >= 0 && Coords.Y < DimensionsInChunks.Y &&
			Coords.Z >= 0 && Coords.Z < DimensionsInChunks.Z;
	}

	FORCEINLINE int32 FVoxelGrid::CalcChunkIndexFromWorldPosition(const FVector& WorldLocation) const
	{
		return CalcChunkIndexFromLocalPosition(Transform.InverseTransformPosition(WorldLocation));
	}

	FORCEINLINE int32 FVoxelGrid::CalcChunkIndexFromLocalPosition(const FVector& LocalPosition) const
	{
		const FIntVector GridPosition = CalcGridCoordsFromLocalPosition(LocalPosition);
		return CoordsToIndex(GridPosition);
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcGridCoordsFromLocalPosition(const FVector& LocalPosition) const
	{
		return FIntVector((LocalPosition + Extent).BoundToBox(FVector::ZeroVector, Extent * 2) / FVoxelEngineConfig::ChunkSize);
	}

	FORCEINLINE FVoxelChunk* FVoxelGrid::GetChunkByIndex(int32 Index)
	{
		if (!Chunks.IsValidIndex(Index))
		{
			return nullptr;
		}
		return &Chunks[Index];
	}

	FORCEINLINE const FVoxelChunk* FVoxelGrid::GetChunkByIndex(int32 Index) const
	{
		return const_cast<FVoxelGrid*>(this)->GetChunkByIndex(Index);
	}

	FORCEINLINE FVoxelChunk& FVoxelGrid::GetChunkRefByIndex(int32 Index)
	{
		checkf(Chunks.IsValidIndex(Index), TEXT("A chunk with index %i does not exist."), Index);
		return Chunks[Index];
	}

	FORCEINLINE const FVoxelChunk& FVoxelGrid::GetChunkRefByIndex(int32 Index) const
	{
		return const_cast<FVoxelGrid*>(this)->GetChunkRefByIndex(Index);
	}

	FORCEINLINE FVoxelChunk* FVoxelGrid::FindChunkByCoords(const FIntVector& Coords)
	{
		if (!AreCoordsValid(Coords))
		{
			return nullptr;
		}
		return &GetChunkRefByIndex(CoordsToIndex(Coords));
	}

	FORCEINLINE const FVoxelChunk* FVoxelGrid::FindChunkByCoords(const FIntVector& Coords) const
	{
		return const_cast<FVoxelGrid*>(this)->FindChunkByCoords(Coords);
	}

	FORCEINLINE FVoxelAddress FVoxelGrid::CalcVoxelAddressFromGlobalCoords(const FIntVector& GlobalCoords)
	{
		ensureMsgf(GlobalCoords.GetMin() >= 0, TEXT("Negative global coords will cause unpredictable results."));
		FVoxelAddress Address;
		Address.ChunkCoords = GlobalCoords / FVoxelChunk::Resolution;
		Address.VoxelCoords = GlobalCoords % FVoxelChunk::Resolution;
		return Address;
	}

	FORCEINLINE FVoxelAddress FVoxelGrid::CalcVoxelAddressFromSignedGlobalCoords(const FIntVector& GlobalCoords)
	{
		FVoxelAddress Address;
		Address.ChunkCoords = CalcChunkCoordsFromSignedGlobalCoords(GlobalCoords);
		const FIntVector AbsChunkCoords{
			FMath::Abs(Address.ChunkCoords.X),
			FMath::Abs(Address.ChunkCoords.Y),
			FMath::Abs(Address.ChunkCoords.Z),
		};
		Address.VoxelCoords = (GlobalCoords + ((AbsChunkCoords + FIntVector(1)) * FVoxelChunk::Resolution)) % FVoxelChunk::Resolution;
		return Address;
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcGlobalCoordsFromVoxelAddress(const FVoxelAddress& VoxelAddress)
	{
		return VoxelAddress.ChunkCoords * FVoxelChunk::Resolution + VoxelAddress.VoxelCoords;
	}

	FORCEINLINE FVoxelAddress FVoxelGrid::CalcVoxelAddressFromGlobalCoordsChecked(const FIntVector& GlobalCoords) const
	{
		FVoxelAddress Address = CalcVoxelAddressFromGlobalCoords(GlobalCoords);
		check(Address.ChunkCoords.X >= 0);
		check(Address.ChunkCoords.Y >= 0);
		check(Address.ChunkCoords.Z >= 0);
		check(Address.ChunkCoords.X < GetDimensionsInChunks().X);
		check(Address.ChunkCoords.Y < GetDimensionsInChunks().Y);
		check(Address.ChunkCoords.Z < GetDimensionsInChunks().Z);
		return Address;
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcGlobalCoordsFromVoxelAddressChecked(const FVoxelAddress& VoxelAddress) const
	{
		FIntVector GlobalCoords = CalcGlobalCoordsFromVoxelAddress(VoxelAddress);
		check(GlobalCoords.X >= 0);
		check(GlobalCoords.Y >= 0);
		check(GlobalCoords.Z >= 0);
		check(GlobalCoords.X < GetDimensionsInChunks().X * FVoxelChunk::Resolution);
		check(GlobalCoords.Y < GetDimensionsInChunks().Y * FVoxelChunk::Resolution);
		check(GlobalCoords.Z < GetDimensionsInChunks().Z * FVoxelChunk::Resolution);
		return GlobalCoords;
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcChunkCoordsFromGlobalCoords(const FIntVector& GlobalCoords)
	{
		ensureMsgf(GlobalCoords.GetMin() >= 0, TEXT("Negative global coords will cause unpredictable results."));
		return GlobalCoords / FVoxelChunk::Resolution;
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcChunkCoordsFromSignedGlobalCoords(const FIntVector& GlobalCoords)
	{
		FIntVector ChunkCoords;
		ChunkCoords.X = GlobalCoords.X < 0 ? ((GlobalCoords.X - (FVoxelChunk::Resolution - 1)) / FVoxelChunk::Resolution) : (GlobalCoords.X / FVoxelChunk::Resolution);
		ChunkCoords.Y = GlobalCoords.Y < 0 ? ((GlobalCoords.Y - (FVoxelChunk::Resolution - 1)) / FVoxelChunk::Resolution) : (GlobalCoords.Y / FVoxelChunk::Resolution);
		ChunkCoords.Z = GlobalCoords.Z < 0 ? ((GlobalCoords.Z - (FVoxelChunk::Resolution - 1)) / FVoxelChunk::Resolution) : (GlobalCoords.Z / FVoxelChunk::Resolution);
		return ChunkCoords;
	}
}
