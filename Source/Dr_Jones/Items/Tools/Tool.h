// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "Tool.generated.h"
class ARuntimeCharacter;

UCLASS()
class DR_JONES_API UTool : public UItem
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void UseItem();

	virtual void BindTool(ARuntimeCharacter* Player);
	virtual void UnbindTool(ARuntimeCharacter* Player);
};
