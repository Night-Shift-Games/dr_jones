// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SurfaceNets_Triangulation.h"
#include "Components/ActorComponent.h"
#include "VoxelEngine.h"

#include "VoxelEngineUObjectInterface.generated.h"

class UDynamicMeshComponent;
class UDynamicMesh;
class UVoxelGrid;
class UVoxelGridVisualizer;

inline TAutoConsoleVariable<bool> CVarVisualizeVoxelGrid(
	TEXT("NS.VE.VisualizeGrid"),
	false,
	TEXT("Show Voxel Grid vizualization in game."),
	ECVF_Cheat
);

class DR_JONES_API FVoxelGridVisualizerSceneProxy final : public FPrimitiveSceneProxy
{
public:
	virtual SIZE_T GetTypeHash() const override { static size_t UniquePointer; return reinterpret_cast<size_t>(&UniquePointer); }

	explicit FVoxelGridVisualizerSceneProxy(const UVoxelGridVisualizer* InComponent);

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual uint32 GetMemoryFootprint() const override { return sizeof(FVoxelGridVisualizerSceneProxy) + GetAllocatedSize(); }
	FORCEINLINE uint32 GetAllocatedSize() const { return Voxels.GetAllocatedSize(); }

	struct FVoxelSceneData
	{
		FVector Location;
		bool bSolid;
	};

private:
	TArray<FVoxelSceneData> Voxels;
	NSVE::FVoxelChunk::FTransformData TransformData;
	bool bDrawDebug;
	bool bDrawSNCells;
	bool bDrawSurfacePoints;
	bool bDrawVoxels;
	TArray<NS::SurfaceNets::Debug::FSurfacePointVis> SurfacePoints;
	int32 CurrentChunkIndex;
};

UCLASS()
class DR_JONES_API UVoxelGridVisualizer : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UVoxelGridVisualizer();

	UVoxelGrid* GetVoxelGrid() const;
	TPair<int32, NSVE::FVoxelChunk*> GetCurrentChunk() const;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	TSharedPtr<NS::SurfaceNets::Debug::FDebugContext> SurfaceNetsDebugContext;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};

USTRUCT(BlueprintType)
struct FVoxelGridGeneratedLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Depth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Material;
};

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent), HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UVoxelGrid : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoxelGrid();

	NSVE::FVoxelGrid& GetInternal() const { check(InternalVoxelGrid.IsValid()); return *InternalVoxelGrid.Get(); }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NightShift|VoxelEngine")
	FVector Extents = FVector(400.0, 400.0, 400.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NightShift|VoxelEngine")
	TArray<FVoxelGridGeneratedLayer> GeneratedLayers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NightShift|VoxelEngine")
	TObjectPtr<UMaterialInterface> ArtifactHintMaterial;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NightShift|VoxelEngine")
	bool bDrawDebug = false;

	UPROPERTY()
	TObjectPtr<UVoxelGridVisualizer> GridVisualizer;
#endif

private:
	TUniquePtr<NSVE::FVoxelGrid> InternalVoxelGrid;
};

UCLASS()
class DR_JONES_API UVoxelEngineUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void TriangulateVoxelGrid_Internal(const NSVE::FVoxelGrid& VoxelGrid, UDynamicMesh* DynamicMesh, int32& OutVertexCount, int32& OutTriangleCount, TFunction<void()> OnCompleted, bool bAsync = true, const TSharedPtr<NS::SurfaceNets::Debug::FDebugContext>& DebugContext = nullptr);

	UFUNCTION(BlueprintCallable, Category = "NightShift|VoxelEngine")
	static void TriangulateVoxelGrid(UVoxelGrid* VoxelGrid, UDynamicMesh* DynamicMesh, int32& OutVertexCount, int32& OutTriangleCount);
};
