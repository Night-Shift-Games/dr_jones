// Property of Night Shift Games, all rights reserved.

#include "VoxelEngineUObjectInterface.h"

#include "SurfaceNets_Triangulation.h"
#include "Triangulation.h"
#include "UDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "DynamicMesh/MeshNormals.h"
#include "DynamicMesh/Operations/MergeCoincidentMeshEdges.h"
#include "GameFramework/Character.h"
#include "Items/Artifacts/Artifact.h"
#include "Kismet/GameplayStatics.h"

FVoxelGridVisualizerSceneProxy::FVoxelGridVisualizerSceneProxy(const UVoxelGridVisualizer* InComponent) :
	FPrimitiveSceneProxy(InComponent),
	SurfacePoints(InComponent->SurfaceNetsDebugContext->SurfacePoints),
	bDrawSurfacePoints(NS::SurfaceNets::Debug::CVar_SurfacePoints.GetValueOnGameThread()),
	bDrawSNCells(NS::SurfaceNets::Debug::CVar_Cells.GetValueOnGameThread()),
	bDrawVoxels(CVarVisualizeVoxelGrid.GetValueOnGameThread()),
	CurrentChunkIndex(-1),
#if WITH_EDITORONLY_DATA
	bDrawDebug(InComponent->GetVoxelGrid()->bDrawDebug)
#else
	bDrawDebug(false)
#endif
{
	using namespace NSVE;

	if (!bDrawDebug)
	{
		return;
	}

	if (auto [Index, Chunk] = InComponent->GetCurrentChunk(); Chunk)
	{
		CurrentChunkIndex = Index;
		TransformData = Chunk->MakeTransformData();

		Chunk->Voxels.Iterate([this](const FVoxel& Voxel, int32 Index, const FIntVector3& Coords)
		{
			const FVector Location = FVoxelChunk::GridPositionToWorld_Static(Coords, TransformData);
			FVoxelSceneData& VoxelSceneData = Voxels.Emplace_GetRef();
			VoxelSceneData.Location = Location;
			VoxelSceneData.bSolid = Voxel.bSolid;
		});
	}
	else
	{
		bDrawSNCells = false;
	}
}

void FVoxelGridVisualizerSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_VoxelGridVisualizerSceneProxy_GetDynamicMeshElements);

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (!(VisibilityMap & (1 << ViewIndex)))
		{
			continue;
		}

		const FSceneView* View = Views[ViewIndex];
		FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

		if (bDrawVoxels)
		{
			for (int32 i = 0; i < Voxels.Num(); i++)
			{
				const FVoxelSceneData& Voxel = Voxels[i];
				const bool bShowNonSolid = true;
				if (!bShowNonSolid && !Voxel.bSolid)
				{
					continue;
				}

				PDI->DrawPoint(Voxel.Location,
					Voxel.bSolid ? FLinearColor(1.0f, 1.0f, 1.0f) : FLinearColor(0.0f, 0.0f, 0.0f, 0.4f),
					Voxel.bSolid ? 8.0f : 2.0f,
					SDPG_World);
			}
		}

		if (bDrawSurfacePoints)
		{
			for (const NS::SurfaceNets::Debug::FSurfacePointVis& Vis : SurfacePoints)
			{
				PDI->DrawPoint(Vis.Location, FColor::Red, 8.0f, SDPG_World);
			}
		}

		if (bDrawSNCells)
		{
			const FVector BaseLocation = TransformData.RootCenter - TransformData.RootExtent + TransformData.VoxelSize / 2.0;
			for (const NS::SurfaceNets::Debug::FSurfacePointVis& Vis : SurfacePoints)
			{
				if (Vis.ChunkIndex == CurrentChunkIndex && Vis.CellCoords.GetMax() < NSVE::FVoxelChunk::Resolution)
				{
					const FVector LocalMin = FVector(Vis.CellCoords.X * TransformData.VoxelSize, Vis.CellCoords.Y * TransformData.VoxelSize, Vis.CellCoords.Z * TransformData.VoxelSize);
					const FVector LocalMax = FVector((Vis.CellCoords.X + 1) * TransformData.VoxelSize, (Vis.CellCoords.Y + 1) * TransformData.VoxelSize, (Vis.CellCoords.Z + 1) * TransformData.VoxelSize);
					const FBox CellBox(BaseLocation + LocalMin, BaseLocation + LocalMax);
					DrawWireBox(PDI, CellBox, FColor::White, SDPG_World, 2.0f, 0, true);
					if (Vis.EdgeIntersections & 1 << 0)
					{
						const FVector Start = BaseLocation + FVector{ LocalMin.X, LocalMax.Y, LocalMax.Z };
						const FVector End   = BaseLocation + FVector{ LocalMax.X, LocalMax.Y, LocalMax.Z };
						PDI->DrawLine(Start, End, FColor::Red, SDPG_World, 4.0f, 1.0f, true);
					}
					if (Vis.EdgeIntersections & 1 << 1)
					{
						const FVector Start = BaseLocation + FVector{ LocalMax.X, LocalMin.Y, LocalMax.Z };
						const FVector End   = BaseLocation + FVector{ LocalMax.X, LocalMax.Y, LocalMax.Z };
						PDI->DrawLine(Start, End, FColor::Green, SDPG_World, 4.0f, 1.0f, true);
					}
					if (Vis.EdgeIntersections & 1 << 2)
					{
						const FVector Start = BaseLocation + FVector{ LocalMax.X, LocalMax.Y, LocalMin.Z };
						const FVector End   = BaseLocation + FVector{ LocalMax.X, LocalMax.Y, LocalMax.Z };
						PDI->DrawLine(Start, End, FColor::Blue, SDPG_World, 4.0f, 1.0f, true);
					}
					if (!Voxels.IsEmpty() && Vis.CellCoords.GetMax() < NSVE::FVoxelChunk::Resolution)
					{
						for (int32 CI = 0; CI < 8; ++CI)
						{
							FIntVector CornerCoords = Vis.CellCoords;
							CornerCoords.X += !!(CI & 1 << 0);
							CornerCoords.Y += !!(CI & 1 << 1);
							CornerCoords.Z += !!(CI & 1 << 2);
							if (CornerCoords.GetMax() < NSVE::FVoxelChunk::Resolution)
							{
								const int32 VoxelIndex = NSVE::Utils::InlineGridPositionToIndex(CornerCoords, NSVE::FVoxelChunk::Resolution);
								check(Voxels.IsValidIndex(VoxelIndex));
								const FVoxelSceneData& Voxel = Voxels[VoxelIndex];
								PDI->DrawPoint(Voxel.Location, Voxel.bSolid ? FColor::White : FColor::Black, 10.0f, SDPG_World);
							}
						}
					}
				}
			}
		}
	}
}

FPrimitiveViewRelevance FVoxelGridVisualizerSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance ViewRelevance;
	ViewRelevance.bDrawRelevance = IsShown(View);
	ViewRelevance.bDynamicRelevance = true;
	ViewRelevance.bSeparateTranslucency = ViewRelevance.bNormalTranslucency = true;
	return ViewRelevance;
}

UVoxelGridVisualizer::UVoxelGridVisualizer()
{
#if WITH_EDITORONLY_DATA
	SetIsVisualizationComponent(true);
#endif

	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	bUseEditorCompositing = true;
	bIgnoreStreamingManagerUpdate = true;

	SurfaceNetsDebugContext = MakeShared<NS::SurfaceNets::Debug::FDebugContext>();
}

UVoxelGrid* UVoxelGridVisualizer::GetVoxelGrid() const
{
	return Cast<UVoxelGrid>(GetOuter());
}

TPair<int32, NSVE::FVoxelChunk*> UVoxelGridVisualizer::GetCurrentChunk() const
{
	using namespace NSVE;

	FVoxelGrid& VoxelGrid = GetVoxelGrid()->GetInternal();
	const UWorld* World = GetWorld();
	const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!Character)
	{
		return { -1, nullptr };
	}

	for (auto It = VoxelGrid.GetChunks().CreateConstIterator(); It; ++It)
	{
		const int32 ChunkIndex = It.GetIndex();
		FVoxelChunkBounds WorldBounds = VoxelGrid.CalcChunkWorldBoundsFromIndex(ChunkIndex);

		FVector Location;
		FRotator Rotation;
		Character->GetActorEyesViewPoint(Location, Rotation);

		const bool bIsInside = WorldBounds.GetBox().IsInside(Location + Rotation.Vector() * 300.0f);
		if (bIsInside)
		{
			FVoxelChunk* Chunk = VoxelGrid.GetChunkByIndex(ChunkIndex);
			return { ChunkIndex, Chunk };
		}
	}

	return { -1, nullptr };
}

FBoxSphereBounds UVoxelGridVisualizer::CalcBounds(const FTransform& LocalToWorld) const
{
	const NSVE::FVoxelGrid& VoxelGrid = GetVoxelGrid()->GetInternal();
	FBoxSphereBounds GridBounds;
	GridBounds.Origin = VoxelGrid.GetTransform().GetLocation();
	GridBounds.BoxExtent = VoxelGrid.GetExtent();
	GridBounds.SphereRadius = GridBounds.BoxExtent.Size();
	return GridBounds;
}

void UVoxelGridVisualizer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MarkRenderStateDirty();
}

FPrimitiveSceneProxy* UVoxelGridVisualizer::CreateSceneProxy()
{
	return new FVoxelGridVisualizerSceneProxy(this);
}

UVoxelGrid::UVoxelGrid()
{
#if ENABLE_VOXEL_ENGINE_DEBUG
	PrimaryComponentTick.bCanEverTick = true;
#else
	PrimaryComponentTick.bCanEverTick = false;
#endif

	InternalVoxelGrid = MakeUnique<NSVE::FVoxelGrid>();

#if WITH_EDITORONLY_DATA
	GridVisualizer = CreateEditorOnlyDefaultSubobject<UVoxelGridVisualizer>(TEXT("VoxelGridVisualizer"));
#endif
}

void UVoxelGrid::BeginPlay()
{
	using namespace NSVE;

	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVoxelGridInitializer Initializer;
	Initializer.Transform = Owner->GetTransform();
	Initializer.Bounds = FBoxSphereBounds(Initializer.Transform.GetLocation(), Extents, Extents.Size());
	if (!GeneratedLayers.IsEmpty())
	{
		const double BaseZ = Initializer.Transform.GetLocation().Z;
		for (int32 I = 0; I < GeneratedLayers.Num(); ++I)
		{
			const FVoxelGridGeneratedLayer& Layer = GeneratedLayers[I];
			FVoxelLayer& VoxelLayer = Initializer.Layers.AddDefaulted_GetRef();
			VoxelLayer.PlaneMaxZ = BaseZ + Layer.Depth;
			VoxelLayer.LocalMaterialIndex = I;
		}
	}
	else
	{
		Initializer.FillSurfaceZ_WS = Initializer.Transform.GetLocation().Z;
	}

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps,
		Initializer.Transform.GetLocation(),
		Initializer.Transform.GetRotation(),
		QueryParams,
		FCollisionShape::MakeBox(Extents));

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (const AArtifact* Artifact = Cast<AArtifact>(Overlap.GetActor()))
		{
			Initializer.ArtifactLocations.Add(Artifact->GetActorLocation());
		}
	}

	check(InternalVoxelGrid);
	InternalVoxelGrid->Initialize(Initializer);
}

void UVoxelGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if ENABLE_VOXEL_ENGINE_DEBUG && WITH_EDITORONLY_DATA
	if (bDrawDebug && CVarVisualizeVoxelGrid.GetValueOnGameThread())
	{
		check(InternalVoxelGrid);

		const UWorld* World = GetWorld();
		check(World);

		FVector LocationToCheck = FVector::ZeroVector;
		if (const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
		{
			LocationToCheck = Character->GetActorLocation();
		}

		InternalVoxelGrid->DrawDebugChunks(*World, LocationToCheck);
	}
#endif
}

void UVoxelEngineUtilities::TriangulateVoxelGrid_Internal(const NSVE::FVoxelGrid& VoxelGrid,
	UDynamicMesh* DynamicMesh,
	int32& OutVertexCount,
	int32& OutTriangleCount,
	TFunction<void()> OnCompleted,
	bool bAsync,
	const TSharedPtr<NS::SurfaceNets::Debug::FDebugContext>& DebugContext)
{
	using namespace NSVE;
	using namespace NSVE::Triangulation;
	using namespace MarchingCubes;
	using namespace UE::Geometry;

	SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal, FColorList::Quartz)

	auto EditDynamicMeshFn = [DynamicMesh, OnCompleted](const TArray<FVector>& CombinedVertices, const TArray<FTriangle>& CombinedTriangles)
	{
		{
			SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal_EditMesh, FColorList::NavyBlue)

			DynamicMesh->EditMesh([&](FDynamicMesh3& EditMesh)
			{
				EditMesh.Clear();

				if (!EditMesh.HasAttributes())
				{
					EditMesh.EnableAttributes();
				}
				if (!EditMesh.Attributes()->HasMaterialID())
				{
					EditMesh.Attributes()->EnableMaterialID();
				}

				{
					SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal_EditMesh_Append, FColorList::DustyRose)
					for (const FVector& Vertex : CombinedVertices)
					{
						EditMesh.AppendVertex(Vertex);
					}
					UE::Geometry::FDynamicMeshMaterialAttribute* MaterialIDs = EditMesh.Attributes()->GetMaterialID();
					for (const FTriangle& Triangle : CombinedTriangles)
					{
						const int32 TriangleID = EditMesh.AppendTriangle(Triangle.A, Triangle.B, Triangle.C);
						// NOTE: With surface nets, non-manifold triangles are to be expected,
						// but of course, Unreal's dynamic mesh shows them a middle finger and doesn't add them at all.
						if (!ensureMsgf(TriangleID >= 0, TEXT("Non-manifold or invalid triangle detected during EditMesh.")))
						{
							continue;
						}
						MaterialIDs->SetValue(TriangleID, static_cast<int32>(Triangle.MaterialIndex));
					}
				}

				{
					// TODO: Optimize: do this earlier => This thing takes like 90% of the whole edit mesh
					// SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal_EditMesh_MergeEdges, FColorList::DustyRose)
					// UE::Geometry::FMergeCoincidentMeshEdges MergeEdges(&EditMesh);
					// MergeEdges.Apply();
				}

				{
					SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal_EditMesh_ComputeNormals, FColorList::DustyRose)
					UE::Geometry::FMeshNormals::QuickComputeVertexNormals(EditMesh);
				}
			}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::Unknown);
		}

		OnCompleted();
	};

	if (bAsync)
	{
		OutVertexCount = 0;
		OutTriangleCount = 0;
		AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [EditDynamicMeshFn,
			// TODO: Kamilu to jebnie
			&VoxelGrid,
			DynamicMesh,
			OnCompleted,
			DebugContext]
		{
			TArray<FVector> CombinedVerticesAsync;
			TArray<FTriangle> CombinedTrianglesAsync;
			// int32 VertexCount;
			// int32 TriangleCount;
			// TriangulateVoxelGrid_MarchingCubes(VoxelGrid, CombinedVerticesAsync, CombinedTrianglesAsync, VertexCount, TriangleCount);
			TArray<FIndex3i> Indices;

			NS::SurfaceNets::TriangulateVoxelGrid(VoxelGrid, CombinedVerticesAsync, Indices, DebugContext.Get());

			Algo::Transform(Indices, CombinedTrianglesAsync, [](const FIndex3i& Triangle)
			{
				FTriangle Triangle2;
				Triangle2.MaterialIndex = 0;
				Triangle2.A = Triangle.A;
				Triangle2.B = Triangle.B;
				Triangle2.C = Triangle.C;
				return Triangle2;
			});

			if (!DynamicMesh)
			{
				OnCompleted();
				return;
			}

			AsyncTask(ENamedThreads::GameThread, [EditDynamicMeshFn,
				CombinedVerticesAsync = MoveTemp(CombinedVerticesAsync),
				CombinedTrianglesAsync = MoveTemp(CombinedTrianglesAsync),
				DebugContext]
			{
				EditDynamicMeshFn(CombinedVerticesAsync, CombinedTrianglesAsync);
				// NS::SurfaceNets::Debug::DrawSurfaceNetsDebug(*DebugContext);
			});
		});
	}
	else
	{
		TArray<FVector> CombinedVertices;
		TArray<FTriangle> CombinedTriangles;
		// TriangulateVoxelGrid_MarchingCubes(VoxelGrid, CombinedVertices, CombinedTriangles, OutVertexCount, OutTriangleCount);
		TArray<FIndex3i> Indices;
		NS::SurfaceNets::TriangulateVoxelGrid(VoxelGrid, CombinedVertices, Indices, DebugContext.Get());
		Algo::Transform(Indices, CombinedTriangles, [](const FIndex3i& Triangle)
		{
			FTriangle Triangle2;
			Triangle2.MaterialIndex = 0;
			Triangle2.A = Triangle.A;
			Triangle2.B = Triangle.B;
			Triangle2.C = Triangle.C;
			return Triangle2;
		});

		if (DynamicMesh)
		{
			EditDynamicMeshFn(CombinedVertices, CombinedTriangles);
			// NS::SurfaceNets::Debug::DrawSurfaceNetsDebug(*DebugContext);
		}
		else
		{
			OnCompleted();
		}
	}
}

void UVoxelEngineUtilities::TriangulateVoxelGrid(UVoxelGrid* VoxelGrid, UDynamicMesh* DynamicMesh, int32& OutVertexCount, int32& OutTriangleCount)
{
	SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid, FColorList::Quartz)

	if (!VoxelGrid)
	{
		UE_LOG(LogDrJones, Error, TEXT("Cannot triangulate a null VoxelGrid"));
		return;
	}

	OutVertexCount = 0;
	OutTriangleCount = 0;
	int32 VertexCount;
	int32 TriangleCount;
	TriangulateVoxelGrid_Internal(VoxelGrid->GetInternal(), DynamicMesh, VertexCount, TriangleCount, []{}, true);
}
