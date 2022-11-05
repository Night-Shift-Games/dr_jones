// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationSegment.h"
#include "DrawDebugHelpers.h"
#include "ComponentVisualizer.h"
#include "ExcavationArea.generated.h"

UCLASS()
class DR_JONES_API AExcavationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AExcavationArea();
	
	USceneComponent* Root;

	TArray<UExcavationSegment*> ExcavationSegments;

	UFUNCTION(CallInEditor)
	void CreateArea();

	UFUNCTION(CallInEditor)
	void RefreshArena();

	UPROPERTY(EditAnywhere)
	float Size;

	UPROPERTY(EditAnywhere)
	int AreaResolution;

	UPROPERTY(EditAnywhere)
	int Resolution;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ExcavateMaterial;

protected:

	virtual void BeginPlay() override;

private:

	void DestroyArea();
	bool bIsArenaGenerated;
	float SegmentSize;
};
