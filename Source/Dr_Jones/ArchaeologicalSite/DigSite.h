// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationSegment.h"
#include "Components/DynamicMeshComponent.h"

#include "DigSite.generated.h"

class UVoxelGrid;

UCLASS()
class DR_JONES_API ADigSite : public AActor
{
	GENERATED_BODY()

public:
	ADigSite();

protected:
	virtual void BeginPlay() override;

	void SetupDigSite(const FVector& DigSiteLocation);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UVoxelGrid> VoxelGrid;

	double Width = 1500.0;
	double Height = 1500.0; 
};
