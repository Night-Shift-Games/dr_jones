// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DynamicMeshComponent.h"

#include "DigSite.generated.h"

class AArtifact;

namespace NSVE
{
	class FVoxelGrid;
	class FVoxelChunk;
}

class UVoxelGrid;

USTRUCT(BlueprintType)
struct FDigSiteVoxelData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 MaterialIndex = 0;
};

USTRUCT(BlueprintType)
struct FDigSiteSample
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FDigSiteVoxelData> Voxels;

	UPROPERTY(BlueprintReadOnly);
	TArray<AArtifact*> Artifacts;
};

UCLASS()
class DR_JONES_API ADigSite : public AActor
{
	GENERATED_BODY()

public:
	ADigSite();

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void Dig(const FVector& Location, float DigRadius);

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	FDigSiteSample SampleDig(const FVector& Location, float DigRadius);

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void UnDig(const FVector& Location, float DigRadius);

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void UpdateMesh(bool bAsync = true);

	void GetChunksInRadius(const FVector& Location, float Radius, TArray<NSVE::FVoxelChunk*>& OutChunks) const;

protected:
	virtual void BeginPlay() override;

	void SetupDigSite(const FVector& DigSiteLocation);
	void SampleVoxelsInRadius(const NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius, TArray<FDigSiteVoxelData>& OutVoxels, TArray<AArtifact*>& OutHitArtifacts) const;
	static void DigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius);
	static void UnDigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius);

	void SpawnArtifacts();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UVoxelGrid> VoxelGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TArray<TSubclassOf<AArtifact>> Artifacts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	int ArtifactSpawnRate = 15.f;
	
	double Width = 1500.0;
	double Height = 1500.0;
};
