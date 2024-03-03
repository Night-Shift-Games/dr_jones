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
class DR_JONES_API UNightShiftDynamicMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:
	UNightShiftDynamicMeshComponent();

	virtual int32 GetNumMaterials() const override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	TArray<FVector> Vertices;
	TArray<FColor> Colors;
	TArray<UE::Geometry::FIndex3i> Triangles;
};
