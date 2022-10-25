// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ExcavationSegment.generated.h"

/**
 * 
 */
UCLASS()
class DR_JONES_API UExcavationSegment : public UProceduralMeshComponent
{
	GENERATED_BODY()
	

public:
	UExcavationSegment(const FObjectInitializer& ObjectInitializer);

	void GenerateMesh(int Resolution, float Size);
	void Dig(FTransform CollisionPoint);

	void RefreshMesh();
	
	
	TArray<FVector> vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
};
