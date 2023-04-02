// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationSegment.h"
#include "Items/Artifacts/Artifact.h"
#include "ArchaeologicalSite.generated.h"

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:	
	AArchaeologicalSite();

	void PopulateWithArtefacts();

	void SpawnArtefact(TSubclassOf<AArtifact>ArtefactClass = nullptr);

	void SpawnAllQuestArtefacts();

	UFUNCTION(CallInEditor)
	void CreateArea();

	UFUNCTION(CallInEditor)
	void RefreshArena();
	
	UPROPERTY(EditAnywhere)
	TArray <TSubclassOf<AArtifact>> ArtefactsClass;
	
	UPROPERTY(EditAnywhere)
	TArray <AArtifact*> Artefacts;
	
	UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
	FVector ArtefactSpawnOrigin;
	
	UPROPERTY(EditAnywhere)
	float ArtefactSpawnAreaX;

	UPROPERTY(EditAnywhere)
	float ArtefactSpawnAreaY;

	UPROPERTY(EditAnywhere)
	int ArtefactsQuantity;

	UPROPERTY(EditAnywhere)
	float Size;

	UPROPERTY(EditAnywhere)
	int AreaResolution;

	UPROPERTY(EditAnywhere)
	int Resolution;

	UPROPERTY(EditAnywhere)
	bool bSmoothDigging;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ExcavateMaterial;

	USceneComponent* Root;

	TArray<UExcavationSegment*> ExcavationSegments;

protected:

	virtual void BeginPlay() override;

	//virtual void OnConstruction(const FTransform& Transform) override;

private:

	void DestroyArea();
	bool bIsArenaGenerated;
	float SegmentSize;
};
