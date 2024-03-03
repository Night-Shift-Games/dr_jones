// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "IndexTypes.h"

#include "NightShiftDynamicMeshComponent.generated.h"

namespace UE::Geometry
{
	struct FIndex3i;
}

UCLASS(ClassGroup = (NightShift), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UNightShiftDynamicMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()

public:
	UNightShiftDynamicMeshComponent();

	// UMeshComponent overrides
	virtual int32 GetNumMaterials() const override;
	// END of UMeshComponent overrides

	// UPrimitiveComponent overrides
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual UBodySetup* GetBodySetup() override;
	// END of UPrimitiveComponent overrides

	// USceneComponent overrides
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	// END of USceneComponent overrides

	// IInterface_CollisionDataProvider overrides
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	// END of IInterface_CollisionDataProvider overrides

	UBodySetup* CreateBodySetupHelper();

	void UpdateCollision();

	TArray<FVector> Vertices;
	TArray<FColor> Colors;
	TArray<UE::Geometry::FIndex3i> Triangles;

	UPROPERTY(Instanced)
	TObjectPtr<UBodySetup> BodySetup;
};
