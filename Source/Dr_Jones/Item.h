// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.generated.h"


UCLASS( ClassGroup=(Item), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UItem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditAnywhere)
	FName Name;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;

};
