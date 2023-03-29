// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryItem.h"

#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = "Storage", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static uint32 WrapIndexToArray(int32 Index, uint16 ArraySize);

protected:
	TArray<TObjectPtr<FInventoryItem>> Items;
};
