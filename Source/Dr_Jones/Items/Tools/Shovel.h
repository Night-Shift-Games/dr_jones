// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "../../Archeological Site/ExcavationArea.h"
#include "../../RuntimeCharacter.h"
#include "Shovel.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Item), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UShovel : public UTool
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	UFUNCTION()
	void Dig();
	virtual void SetupTool() override;
protected:
	bool bFilled;
};
