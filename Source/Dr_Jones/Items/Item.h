// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Item.generated.h"

class UInteractableComponent;

UCLASS(HideCategories = (Rendering, Replication, Collision, HLOD, Input, "Code View", Cooking, Actor))
class DR_JONES_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UInteractableComponent> InteractableComponent;
};
