// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "ExcavationArea.generated.h"

UCLASS()
class DR_JONES_API AExcavationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AExcavationArea();

protected:
	virtual void BeginPlay() override;

	virtual void PostActorCreated() override;

public:
	UProceduralMeshComponent* PMC;
	
	UFUNCTION(CallInEditor)
	void CreateMesh();
	
	UFUNCTION(CallInEditor)
	void RefreshMesh();

	UFUNCTION(CallInEditor)
	void Dig();

	virtual void PostLoad() override;

	UPROPERTY(EditAnywhere)
	int Resolution;

	UPROPERTY(EditAnywhere)
	int Size;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ExcavateMaterial;

	UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
	FTransform CollisionBox;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private: 
	TArray<FVector> vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
};
