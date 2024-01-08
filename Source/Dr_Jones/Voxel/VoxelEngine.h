// Property of Night Shift Games, all rights reserved.

#pragma once

#define ENABLE_VOXEL_ENGINE_DEBUG (ENABLE_VISUAL_LOG)

DECLARE_STATS_GROUP(TEXT("VoxelEngine"), STATGROUP_VoxelEngine, STATCAT_Advanced);

namespace NSVE
{
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
	};

	// Raw packed data of a single voxel
	struct FVoxel
	{
		union
		{
			struct
			{
				uint8 bSolid : 1;
				uint8 LocalMaterial : 3; // 8 materials per chunk (or maybe even per level) should be enough
			};
			uint8 Data;
		};
	};

	// Global compile-time data used by the voxel engine
	struct FVoxelEngineConfig
	{
		// Size of the chunk in UU. Every chunk is a cube
		static constexpr double ChunkSize = 200.0;
		static constexpr double HalfChunkSize = ChunkSize / 2.0;

		// How many voxels are in each chunk (in one dimension)
		static constexpr int32 ChunkResolution = 1 << 4;

		// Max count of voxels in a single voxel chunk
		static constexpr int32 ChunkMaxVoxelCount = ChunkResolution * ChunkResolution * ChunkResolution;

		// Max size of raw allocated data of a single voxel chunk
		static constexpr int32 ChunkMaxSizeInBytes = ChunkMaxVoxelCount * sizeof(FVoxel);
	};

	inline constexpr size_t Alignment = std::hardware_constructive_interference_size;

	// Inline array of voxels with utility operations
	class DR_JONES_API alignas(Alignment) FVoxelArray
	{
	public:
		using FElement = FVoxel;
		static constexpr size_t ElementSize = sizeof(FElement);
		static constexpr int32 ElementCount = FVoxelEngineConfig::ChunkMaxVoxelCount;

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

			for (int32 Index = 0; Index < ElementCount; ++Index)
			{
				check(IsValidIndex(Index));
				FIntVector Coords = IndexToCoords(Index);
				FElement& Voxel = Voxels[Index];
				IterateFunc(Voxel, Index, Coords);
			}
		}

		// Iterate through all voxels using a function with signature void(const FVoxel& Voxel, int32 Index, const FIntVector& Coords)
		template <typename Func>
		void Iterate(Func IterateFunc) const
		{
			SCOPED_NAMED_EVENT(VoxelEngine_VoxelArray_IterateVoxels, FColorList::MandarianOrange)
			DECLARE_SCOPE_CYCLE_COUNTER(TEXT("VoxelEngine::FVoxelArray::IterateVoxels"), STAT_VoxelEngine_FVoxelArray_IterateVoxels, STATGROUP_VoxelEngine)

			for (int32 Index = 0; Index < ElementCount; ++Index)
			{
				check(IsValidIndex(Index));
				FIntVector Coords = IndexToCoords(Index);
				const FElement& Voxel = Voxels[Index];
				IterateFunc(Voxel, Index, Coords);
			}
		}

		// Iterate through all voxels using a function with signature void(FVoxel& Voxel, int32 Index)
		// Version without precalculated coords
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
		// Version without precalculated coords
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

		static FIntVector3 GetDimensions() { return FIntVector3(FVoxelEngineConfig::ChunkResolution); }

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

	// Single voxel chunk that exists in a voxel grid
	// Used to store data in segments that can be loaded/unloaded on demand only when needed
	class DR_JONES_API alignas(Alignment) FVoxelChunk
	{
	public:
		FVoxelChunkBounds Bounds;
		FVoxelArray Voxels;

	public:
		explicit FVoxelChunk(const FVoxelChunkBounds& Bounds);

		void Clear();
		void FillTest();
		void FillSurface(float SurfaceZ_WS);

		struct FTransformData
		{
			FVector RootCenter;
			FVector::FReal RootExtent;
			FVector::FReal VoxelSize;
		};

		FTransformData MakeTransformData() const;

		FVector GridPositionToWorld(const FIntVector& LocalPosition) const;
		static FVector GridPositionToWorld_Static(const FIntVector& LocalPosition, const FTransformData& TransformData);

		FIntVector WorldPositionToGrid(const FVector& WorldPosition) const;
		static FIntVector WorldPositionToGrid_Static(const FVector& WorldPosition, const FTransformData& TransformData);

		static FIntVector IndexToCoords(int32 Index);
		static int32 CoordsToIndex(const FIntVector& Coords);

#if ENABLE_VOXEL_ENGINE_DEBUG
		void DrawDebugVoxels() const;
#endif
	};

	struct FVoxelGridInitializer
	{
		FTransform Transform;
		FBoxSphereBounds Bounds;
		// TODO: Make some fill initializer
		float FillSurfaceZ_WS;
	};

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
			FVoxelChunk* FirstChunk = Chunks.GetData();
			const int32 ChunkCount = DimensionsInChunks.X * DimensionsInChunks.Y * DimensionsInChunks.Z;
			ParallelForTemplate(ChunkCount, [this, ForEachChunk, FirstChunk](int32 Index)
			{
				ForEachChunk(*(FirstChunk + Index), Index);
			});
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
		FVoxelChunkBounds CalcChunkWorldBoundsFromGridPosition(const FIntVector& GridPosition) const;

		int32 CalcChunkIndexFromWorldPosition(const FVector& WorldLocation) const;
		int32 CalcChunkIndexFromLocalPosition(const FVector& LocalPosition) const;
		FIntVector CalcGridPositionFromLocalPosition(const FVector& LocalPosition) const;

		FIntVector IndexToCoords(int32 Index) const;
		int32 CoordsToIndex(const FIntVector& Coords) const;

		FVoxelChunk* GetChunkByIndex(int32 Index);
		const FVoxelChunk* GetChunkByIndex(int32 Index) const;
		FVoxelChunk& GetChunkRefByIndex(int32 Index);
		const FVoxelChunk& GetChunkRefByIndex(int32 Index) const;

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
			static constexpr int32 LowBound = FVoxelEngineConfig::ChunkResolution * 1 / 3;
			static constexpr int32 HighBound = FVoxelEngineConfig::ChunkResolution * 2 / 3;
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
		return Utils::IndexToInlineGridPosition(Index, FVoxelEngineConfig::ChunkResolution);
	}

	FORCEINLINE constexpr int32 FVoxelArray::CoordsToIndex(const FIntVector& Coords)
	{
		return Utils::InlineGridPositionToIndex(Coords, FVoxelEngineConfig::ChunkResolution);
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
		TransformData.VoxelSize = (TransformData.RootExtent * 2.0) / FVoxelEngineConfig::ChunkResolution;
		return TransformData;
	}

	FORCEINLINE FVector FVoxelChunk::GridPositionToWorld(const FIntVector& LocalPosition) const
	{
		return GridPositionToWorld_Static(LocalPosition, MakeTransformData());
	}

	FORCEINLINE FVector FVoxelChunk::GridPositionToWorld_Static(const FIntVector& LocalPosition, const FTransformData& TransformData)
	{
		const FVector WorldPositionNoOffset = FVector(LocalPosition.X, LocalPosition.Y, LocalPosition.Z) * TransformData.VoxelSize;
		const FVector Offset = -FVector(TransformData.RootExtent - TransformData.VoxelSize / 2.0);
		const FVector Location = TransformData.RootCenter + WorldPositionNoOffset + Offset;
		return Location;
	}

	FORCEINLINE FIntVector FVoxelChunk::WorldPositionToGrid(const FVector& WorldPosition) const
	{
		return WorldPositionToGrid_Static(WorldPosition, MakeTransformData());
	}

	FORCEINLINE FIntVector FVoxelChunk::WorldPositionToGrid_Static(const FVector& WorldPosition, const FTransformData& TransformData)
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

	// -------------------------- FVoxelChunk ----------------------------------------------------------------------

	FORCEINLINE FVoxelChunkBounds FVoxelGrid::CalcChunkWorldBoundsFromIndex(int32 Index) const
	{
		const FIntVector LocalPosition = IndexToCoords(Index);
		return CalcChunkWorldBoundsFromGridPosition(LocalPosition);
	}

	FORCEINLINE FVoxelChunkBounds FVoxelGrid::CalcChunkWorldBoundsFromGridPosition(const FIntVector& GridPosition) const
	{
		const FVector RelativePosition = FVector(GridPosition * FVoxelEngineConfig::ChunkSize) + FVoxelEngineConfig::HalfChunkSize - Extent;
		const FVector ChunkOrigin = Transform.TransformPosition(RelativePosition);
		return FVoxelChunkBounds{ ChunkOrigin, FVoxelEngineConfig::HalfChunkSize };
	}

	FORCEINLINE FIntVector FVoxelGrid::IndexToCoords(int32 Index) const
	{
		return Utils::IndexToInlineGridPosition(Index, DimensionsInChunks);
	}

	FORCEINLINE int32 FVoxelGrid::CoordsToIndex(const FIntVector& Coords) const
	{
		return Utils::InlineGridPositionToIndex(Coords, DimensionsInChunks);
	}

	FORCEINLINE int32 FVoxelGrid::CalcChunkIndexFromWorldPosition(const FVector& WorldLocation) const
	{
		return CalcChunkIndexFromLocalPosition(Transform.InverseTransformPosition(WorldLocation));
	}

	FORCEINLINE int32 FVoxelGrid::CalcChunkIndexFromLocalPosition(const FVector& LocalPosition) const
	{
		const FIntVector GridPosition = CalcGridPositionFromLocalPosition(LocalPosition);
		return CoordsToIndex(GridPosition);
	}

	FORCEINLINE FIntVector FVoxelGrid::CalcGridPositionFromLocalPosition(const FVector& LocalPosition) const
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
		FVoxelChunk* Chunk = GetChunkByIndex(Index);
		check(Chunk);
		return *Chunk;
	}

	FORCEINLINE const FVoxelChunk& FVoxelGrid::GetChunkRefByIndex(int32 Index) const
	{
		return const_cast<FVoxelGrid*>(this)->GetChunkRefByIndex(Index);
	}
}
