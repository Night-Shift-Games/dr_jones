// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"

#include "Bucket.generated.h"

UCLASS()
class DR_JONES_API ABucket : public ATool
{
	GENERATED_BODY()
	
public:

	virtual void UseToolPrimaryAction() override;
};
