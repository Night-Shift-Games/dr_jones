// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationSegment.h"
#include "DrawDebugHelpers.h"
#include "Components/BrushComponent.h"
#include "../Items/Artefacts/Artefact.h"
#include "ExcavationArea.generated.h"

UCLASS()
class DR_JONES_API AExcavationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AExcavationArea();

	void PopulateWithArtefacts();

	void SpawnArtefact(TSubclassOf<UArtefact>ArtefactClass = nullptr);

	void SpawnAllQuestArtefacts();

	UFUNCTION(CallInEditor)
	void CreateArea();

	UFUNCTION(CallInEditor)
	void RefreshArena();
	
	UPROPERTY(EditAnywhere)
	TArray <TSubclassOf<UArtefact>> ArtefactsClass;
	
	UPROPERTY(EditAnywhere)
	TArray <UArtefact*> Artefacts;
	
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
