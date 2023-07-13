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
	FVector CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const;

	UFUNCTION(BlueprintCallable)
	void TestDig(FVector Direction, FTransform SphereOrigin);

	UFUNCTION(BlueprintCallable)
	UDynamicMesh* AllocateDynamicMesh();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
};
