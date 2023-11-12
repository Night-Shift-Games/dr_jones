// Property of Night Shift Games, all rights reserved.

#include "VoxelEngineUObjectInterface.h"

#include "Triangulation.h"
#include "UDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

FVoxelGridVisualizerSceneProxy::FVoxelGridVisualizerSceneProxy(const UVoxelGridVisualizer* InComponent) :
	FPrimitiveSceneProxy(InComponent),
	bDrawDebug(InComponent->GetVoxelGrid()->bDrawDebug)
{
	using namespace VoxelEngine;

	FVoxelChunk* Chunk = InComponent->GetCurrentChunk();
	if (!Chunk)
	{
		return;
	}

	if (!bDrawDebug || !CVarVisualizeVoxelGrid.GetValueOnGameThread())
	{
		return;
	}

	LocalToWorldTransformData = Chunk->MakeLocalToWorldTransformData();
	Chunk->Voxels.Iterate([this](const FVoxel& Voxel, int32 Index, const FIntVector3& Coords)
	{
		const FVector Location = FVoxelChunk::LocalPositionToWorld_Static(Coords, LocalToWorldTransformData);
		FVoxelSceneData& VoxelSceneData = Voxels.Emplace_GetRef();
		VoxelSceneData.Location = Location;
		VoxelSceneData.bSolid = Voxel.bSolid;
	});
}

void FVoxelGridVisualizerSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_VoxelGridVisualizerSceneProxy_GetDynamicMeshElements);

	if (Voxels.IsEmpty())
	{
		return;
	}

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (!(VisibilityMap & (1 << ViewIndex)))
		{
			continue;
		}

		const FSceneView* View = Views[ViewIndex];
		FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

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
				Voxel.bSolid ? 4.0f : 1.0f,
				SDPG_World);
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
	SetIsVisualizationComponent(true);

	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	bUseEditorCompositing = true;
	bIgnoreStreamingManagerUpdate = true;
}

UVoxelGrid* UVoxelGridVisualizer::GetVoxelGrid() const
{
	return Cast<UVoxelGrid>(GetOuter());
}

VoxelEngine::FVoxelChunk* UVoxelGridVisualizer::GetCurrentChunk() const
{
	using namespace VoxelEngine;

	FVoxelGrid* VoxelGrid = GetVoxelGrid()->GetInternal();
	const UWorld* World = GetWorld();
	const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);

	for (auto It = VoxelGrid->GetChunks().CreateConstIterator(); It; ++It)
	{
		const int32 ChunkIndex = It.GetIndex();
		FVoxelChunkBounds WorldBounds = VoxelGrid->CalcChunkWorldBoundsFromIndex(ChunkIndex);

		FVector Location;
		FRotator Rotation;
		Character->GetActorEyesViewPoint(Location, Rotation);

		const bool bIsInside = WorldBounds.GetBox().IsInside(Location + Rotation.Vector() * 300.0f);
		if (bIsInside)
		{
			FVoxelChunk* Chunk = VoxelGrid->GetChunkByIndex(ChunkIndex);
			return Chunk;
		}
	}

	return nullptr;
}

FBoxSphereBounds UVoxelGridVisualizer::CalcBounds(const FTransform& LocalToWorld) const
{
	const VoxelEngine::FVoxelGrid* VoxelGrid = GetVoxelGrid()->GetInternal();
	FBoxSphereBounds GridBounds;
	GridBounds.Origin = VoxelGrid->GetTransform().GetLocation();
	GridBounds.BoxExtent = VoxelGrid->GetExtent();
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

	InternalVoxelGrid = MakeUnique<VoxelEngine::FVoxelGrid>();

#if WITH_EDITORONLY_DATA
	GridVisualizer = CreateEditorOnlyDefaultSubobject<UVoxelGridVisualizer>(TEXT("VoxelGridVisualizer"));
#endif
}

void UVoxelGrid::GenerateMesh(UDynamicMesh* DynamicMesh)
{
	using namespace VoxelEngine;

	if (!DynamicMesh)
	{
		UE_LOG(LogDrJones, Error, TEXT("Called UVoxelGrid::GenerateMesh on a null DynamicMesh."));
		return;
	}

	// InternalVoxelGrid->IterateChunksParallel([DynamicMesh](const FVoxelChunk& Chunk, int32 Index)
	auto EditDynamicMesh = [DynamicMesh](const FVoxelChunk& Chunk, int32 Index)
	{
		auto InsertVertexFunc = [DynamicMesh](const FVector3f& Vertex)
		{
			if (!IsValid(DynamicMesh))
			{
				return;
			}

			DynamicMesh->EditMesh([&](FDynamicMesh3& EditMesh)
			{
				int32 NewVertexIndex = EditMesh.AppendVertex(FVector(Vertex));
			}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::MeshTopology, true);
		};

		auto InsertTriangleFunc = [DynamicMesh](const MarchingCubes::FTriangle& Triangle)
		{
			if (!IsValid(DynamicMesh))
			{
				return;
			}

			DynamicMesh->EditMesh([&](FDynamicMesh3& EditMesh)
			{
				int32 NewTriangleID = EditMesh.AppendTriangle(Triangle.Indices[0], Triangle.Indices[1], Triangle.Indices[2]);
			}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::MeshTopology, true);
		};

		Triangulation::TriangulateVoxelChunk_MarchingCubes(Chunk, InsertVertexFunc, InsertTriangleFunc);

		// Just post an update, can't find a better way to do this
		DynamicMesh->EditMesh([&](FDynamicMesh3&) { });
	};

	const int32 Index = InternalVoxelGrid->CoordsToIndex({5, 3, 4});
	const FVoxelChunk* Chunk = InternalVoxelGrid->GetChunkByIndex(Index);
	check(Chunk);
	EditDynamicMesh(*Chunk, Index);
}

void UVoxelGrid::GenerateMeshForComponent(UDynamicMeshComponent* DynamicMeshComponent)
{
	if (!DynamicMeshComponent)
	{
		UE_LOG(LogDrJones, Error, TEXT("Called UVoxelGrid::GenerateMeshForComponent on a null DynamicMeshComponent."));
		return;
	}

	GenerateMesh(DynamicMeshComponent->GetDynamicMesh());
	DynamicMeshComponent->SetComplexAsSimpleCollisionEnabled(true, false);
	DynamicMeshComponent->UpdateCollision();
}

void UVoxelGrid::BeginPlay()
{
	using namespace VoxelEngine;

	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVoxelGridInitializer Initializer;
	Initializer.Transform = Owner->GetTransform();
	Initializer.Bounds = FBoxSphereBounds(Initializer.Transform.GetLocation(), Extents, Extents.Size());;

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
