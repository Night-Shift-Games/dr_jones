// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoxelEngine.h"

#include "VoxelEngineUObjectInterface.generated.h"

class UDynamicMesh;
class UVoxelGrid;
class UVoxelGridVisualizer;

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
	VoxelEngine::FVoxelChunk::FLocalToWorldTransformData LocalToWorldTransformData;
};

UCLASS()
class DR_JONES_API UVoxelGridVisualizer : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UVoxelGridVisualizer();

	UVoxelGrid* GetVoxelGrid() const;
	VoxelEngine::FVoxelChunk* GetCurrentChunk() const;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent), HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UVoxelGrid : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoxelGrid();

	VoxelEngine::FVoxelGrid* GetInternal() const { return InternalVoxelGrid.Get(); }

	UFUNCTION(BlueprintCallable, Category = "VoxelGrid")
	void GenerateMesh(UDynamicMesh* DynamicMesh);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VoxelGrid")
	FVector Extents = FVector(400.0, 400.0, 400.0);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoxelGrid")
	bool bDrawDebug = false;

	UPROPERTY()
	TObjectPtr<UVoxelGridVisualizer> GridVisualizer;
#endif

private:
	TUniquePtr<VoxelEngine::FVoxelGrid> InternalVoxelGrid;
};
