// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExcavationSegment.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

struct FChunk;

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:
	AArchaeologicalSite();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void ModifyTerrainAt(FTransform ModifyLocation);
	
protected:
	TObjectPtr<UTerrain> Terrain;
	// TObjectPtr<UArtifactSpawner> ArtifactSpawner;
	// TObjectPtr<AIlluminati> Illuminati;
};

UCLASS()
class UTerrain : public UObject
{
	GENERATED_BODY()
public:
	UTerrain() = default;
	void ModifyAt(const FTransform& Location, const FTransform& Shape);
	void CreateSectionAt(const FVector& Location);

	TObjectPtr<AArchaeologicalSite> OwningArchaeologicalSite;
protected:
	TMap<FVector, TObjectPtr<UExcavationSegment>> Segments;

};