// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicMeshComponent.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

struct FChunk
{
	int Hash;
	FColor Color = FColor::Red;
};

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:
	AArchaeologicalSite();
	virtual void Tick(float DeltaSeconds) override;
	
	FVector CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const;

	UFUNCTION(BlueprintCallable)
	void TestDig(FVector Direction, FTransform SphereOrigin);

	UFUNCTION(BlueprintCallable)
	UDynamicMesh* AllocateDynamicMesh();

	UFUNCTION(BlueprintCallable)
	void SampleChunk(FVector Location);
	
	FChunk* GetChunkAtLocation(FVector Location);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	TMap<FIntVector3, TSharedPtr<FChunk>> Chunks;

};