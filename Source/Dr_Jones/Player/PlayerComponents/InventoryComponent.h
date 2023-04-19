// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Items/InventoryItem.h"

#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = "Player Components", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	TArray<TObjectPtr<FInventoryItem>> Items;
};
