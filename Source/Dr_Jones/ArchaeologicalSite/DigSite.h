// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DynamicMeshComponent.h"

#include "DigSite.generated.h"

namespace NSVE
{
	class FVoxelGrid;
	class FVoxelChunk;
}

class UVoxelGrid;

UCLASS()
class DR_JONES_API ADigSite : public AActor
{
	GENERATED_BODY()

public:
	ADigSite();

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void Dig(const FVector& Location, float DigRadius);

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void UnDig(const FVector& Location, float DigRadius);

	UFUNCTION(BlueprintCallable, Category = "Dig Site")
	void UpdateMesh(bool bAsync = true);

protected:
	virtual void BeginPlay() override;

	void SetupDigSite(const FVector& DigSiteLocation);
	static void DigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius);
	static void UnDigVoxelsInRadius(NSVE::FVoxelChunk& Chunk, const FVector& Location, float DigRadius);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UVoxelGrid> VoxelGrid;

	double Width = 1500.0;
	double Height = 1500.0;
};
