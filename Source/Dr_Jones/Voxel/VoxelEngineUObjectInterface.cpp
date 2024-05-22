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
	SurfaceNetsChunkFields(InComponent->SurfaceNetsDebugContext->SurfaceNetsFields),
	bDrawSurfacePoints(NS::SurfaceNets::Debug::CVar_SurfacePoints.GetValueOnGameThread()),
	bDrawSNCells(NS::SurfaceNets::Debug::CVar_Cells.GetValueOnGameThread()),
	bDrawSNCellIntersections(NS::SurfaceNets::Debug::CVar_Cells_Intersections.GetValueOnGameThread()),
	bDrawSNCellSDF(NS::SurfaceNets::Debug::CVar_Cells_SDFValues.GetValueOnGameThread()),
	bDrawVoxels(CVarVisualizeVoxelGrid.GetValueOnGameThread()),
	bDrawSNField(NS::SurfaceNets::Debug::CVar_SurfaceNetsField.GetValueOnGameThread()),
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
			const FVector Location = FVoxelChunk::CoordsToWorld_Static(Coords, TransformData);
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

		const FVector BaseLocation = TransformData.RootCenter - TransformData.RootExtent + TransformData.VoxelSize / 2.0;

		if (bDrawSNCells)
		{
			for (const NS::SurfaceNets::Debug::FSurfacePointVis& Vis : SurfacePoints)
			{
				if (Vis.ChunkIndex == CurrentChunkIndex && Vis.CellCoords.GetMax() < NSVE::FVoxelChunk::Resolution)
				{
					const FVector LocalMin = FVector(Vis.CellCoords.X * TransformData.VoxelSize, Vis.CellCoords.Y * TransformData.VoxelSize, Vis.CellCoords.Z * TransformData.VoxelSize);
					const FVector LocalMax = FVector((Vis.CellCoords.X + 1) * TransformData.VoxelSize, (Vis.CellCoords.Y + 1) * TransformData.VoxelSize, (Vis.CellCoords.Z + 1) * TransformData.VoxelSize);
					const FBox CellBox(BaseLocation + LocalMin, BaseLocation + LocalMax);
					DrawWireBox(PDI, CellBox, FColor::White, SDPG_World, 2.0f, 0, true);
					if (bDrawSNCellIntersections)
					{
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
					}
					if (bDrawSNCellSDF && !Voxels.IsEmpty() && Vis.CellCoords.GetMax() < NSVE::FVoxelChunk::Resolution)
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

		if (bDrawSNField && !SurfaceNetsChunkFields.IsEmpty() && SurfaceNetsChunkFields.IsValidIndex(CurrentChunkIndex))
		{
			TSharedPtr<NS::SurfaceNets::FSurfaceNetsChunkField> Field = SurfaceNetsChunkFields[CurrentChunkIndex];
			Field->Iterate([&, this](float Value, int32 Index, const FIntVector& Coords)
			{
				static constexpr float SizeMultiplier = 10.0f;
				static constexpr float MinSize = 3.0f;

				const FVector Location = BaseLocation + FVector(Coords - FIntVector(2)) * TransformData.VoxelSize;
				const bool bNegative = Value < 0.0f;
				const float AbsValue = FMath::Abs(Value);
				const uint8 ColorValue = FMath::Clamp(static_cast<uint8>(AbsValue * 255.0f), 0, 255);
				const FColor Color = bNegative ? FColor(0, 0, ColorValue) : FColor(ColorValue, 0, 0);
				const float PointSize = FMath::Max(MinSize, AbsValue * SizeMultiplier);
				PDI->DrawPoint(Location, Color, PointSize, SDPG_World);
			});
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
	if (VoxelGrid.GetChunks().IsEmpty())
	{
		// Avoid 0 sized bounds.
		return FBoxSphereBounds(GetComponentLocation(), FVector(1.0), 1.0);
	}

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

void UVoxelGrid::InitializeGrid(const NSVE::FVoxelGridInitializer& Initializer)
{
	using namespace NSVE;



	check(InternalVoxelGrid);
	InternalVoxelGrid->Initialize(Initializer);
}

void UVoxelGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO: Remove this crap and reimplement it in the vis component
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

	auto EditDynamicMeshFn = [DynamicMesh, OnCompleted](
		const TArray<FVector>& CombinedVertices,
		const TArray<FTriangle>& CombinedTriangles,
		const TArray<NS::SurfaceNets::FMaterialWeights>& CombinedMaterialWeights)
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
				if (!EditMesh.Attributes()->HasPrimaryColors())
				{
					EditMesh.Attributes()->EnablePrimaryColors();
				}

				FDynamicMeshColorOverlay* ColorOverlay = EditMesh.Attributes()->PrimaryColors();
				FDynamicMeshMaterialAttribute* MaterialIDs = EditMesh.Attributes()->GetMaterialID();

				{
					SCOPED_NAMED_EVENT(VoxelEngineUtilities_TriangulateVoxelGrid_Internal_EditMesh_Append, FColorList::DustyRose)
					for (auto It = CombinedVertices.CreateConstIterator(); It; ++It)
					{
						const FVector& Vertex = *It;
						EditMesh.AppendVertex(Vertex);
					}

					for (const FTriangle& Triangle : CombinedTriangles)
					{
						const int32 TriangleID = EditMesh.AppendTriangle(Triangle.A, Triangle.B, Triangle.C);
						// NOTE: With surface nets, non-manifold triangles are to be expected,
						// but of course, Unreal's dynamic mesh shows them a middle finger and doesn't add them at all.
						if (!ensureMsgf(TriangleID >= 0, TEXT("Non-manifold or invalid triangle detected during EditMesh.")))
						{
							continue;
						}
						MaterialIDs->SetValue(TriangleID, 0);
					}

					ColorOverlay->CreateFromPredicate([](int, int, int) { return true; }, 0.0f);
					for (const int32 ElementID : ColorOverlay->ElementIndicesItr())
					{
						const int32 VertexID = ColorOverlay->GetParentVertex(ElementID);
						FVector4f Color;
						Color.X = CombinedMaterialWeights[VertexID].Weights[0];
						Color.Y = CombinedMaterialWeights[VertexID].Weights[1];
						Color.Z = CombinedMaterialWeights[VertexID].Weights[2];
						Color.W = CombinedMaterialWeights[VertexID].Weights[7];
						ColorOverlay->SetElement(ElementID, Color);
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
			TArray<NS::SurfaceNets::FMaterialWeights> CombinedMaterialWeights;

			NS::SurfaceNets::TriangulateVoxelGrid(VoxelGrid, CombinedVerticesAsync, Indices, CombinedMaterialWeights, DebugContext.Get());

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
				CombinedMaterialWeights = MoveTemp(CombinedMaterialWeights)]
			{
				EditDynamicMeshFn(CombinedVerticesAsync, CombinedTrianglesAsync, CombinedMaterialWeights);
			});
		});
	}
	else
	{
		TArray<FVector> CombinedVertices;
		TArray<FTriangle> CombinedTriangles;
		// TriangulateVoxelGrid_MarchingCubes(VoxelGrid, CombinedVertices, CombinedTriangles, OutVertexCount, OutTriangleCount);
		TArray<FIndex3i> Indices;
		TArray<NS::SurfaceNets::FMaterialWeights> CombinedMaterialWeights;
		NS::SurfaceNets::TriangulateVoxelGrid(VoxelGrid, CombinedVertices, Indices, CombinedMaterialWeights, DebugContext.Get());
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
			EditDynamicMeshFn(CombinedVertices, CombinedTriangles, CombinedMaterialWeights);
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
