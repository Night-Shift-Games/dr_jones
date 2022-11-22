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
	UItem();
	
	UPROPERTY(EditDefaultsOnly)
	FName ItemName;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* ItemMesh;

protected:
	UStaticMeshComponent* StaticMeshComponent;

};
