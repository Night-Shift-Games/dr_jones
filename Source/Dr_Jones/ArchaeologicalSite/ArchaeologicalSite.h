// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArchaeologicalSite/Chunk.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:
	AArchaeologicalSite();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void TestDig(FVector Direction, FTransform SphereOrigin);

	UFUNCTION(BlueprintCallable)
	void SampleChunk(FVector Location);
	
	FVector CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const;
	FMasterChunk& GetChunkAtLocation(const FVector& Location);
	void DrawChunkDebug();

public:
	TMap<FIntVector3, TSharedPtr<FMasterChunk>> Chunks;

};