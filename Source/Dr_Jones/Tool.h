// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Tool.generated.h"

/**
 * 
 */
UCLASS()
class DR_JONES_API UTool : public UItem
{
	GENERATED_BODY()

public:
		virtual void UseItem();
};
