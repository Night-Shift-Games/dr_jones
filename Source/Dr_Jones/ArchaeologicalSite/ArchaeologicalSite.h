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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	UFUNCTION(BlueprintCallable)
	void TestDig(FTransform SphereOrigin);
	
	UFUNCTION(BlueprintCallable)
	UDynamicMesh* AllocateDynamicMesh();
};
