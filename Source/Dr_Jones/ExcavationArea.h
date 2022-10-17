// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
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

	void CreateFace();

	virtual void PostLoad() override;

	UPROPERTY(EditAnywhere)
	int Resolution;

	UPROPERTY(EditAnywhere)
	int Size;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ExcavateMaterial;

	 
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
