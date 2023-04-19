// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "InventoryItem.generated.h"


USTRUCT(BlueprintType)
struct DR_JONES_API FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item", meta = (DisplayPriority = 2))
	FName ItemID = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item", meta = (DisplayPriority = 3))
	FText ItemDisplayName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item", meta = (DisplayPriority = 1))
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item", meta = (DisplayPriority = 4, MultiLine = true))
	FText Description;
};
