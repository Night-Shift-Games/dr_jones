// Property of Night Shift Games, all rights reserved.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "../../Archeological Site/ExcavationArea.h"
#include "../../RuntimeCharacter.h"
#include "Shovel.generated.h"

UCLASS(ClassGroup = (Item), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UShovel : public UTool
{
	GENERATED_BODY()

public:

	virtual void UseItem() override;

protected:

	void Dig();
	bool bFilled = false;
};