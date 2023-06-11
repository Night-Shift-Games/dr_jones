// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicMeshComponent.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:
	AArchaeologicalSite();
	float CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second) const;
	float GetChordLenght(const float SphereRadius, const FVector& Direction, const FVector& Second) const;
	FVector CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const;
	bool IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, float Radius) const;

	UFUNCTION(BlueprintCallable)
	void TestDig(FVector Direction, FTransform SphereOrigin);

	UFUNCTION(BlueprintCallable)
	UDynamicMesh* AllocateDynamicMesh();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
};
