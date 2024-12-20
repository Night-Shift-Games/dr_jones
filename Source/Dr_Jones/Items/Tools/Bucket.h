// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Tool.h"

#include "Bucket.generated.h"

UCLASS(ClassGroup = (Item), Blueprintable)
class DR_JONES_API ABucket : public ATool
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	int BucketCapacity = 5;

	virtual void OnRemovedFromEquipment() override;
	virtual void OnUnequip() override;
};
