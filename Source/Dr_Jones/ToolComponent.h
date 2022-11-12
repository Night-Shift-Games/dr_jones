// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Storage.h"
#include "Tool.h"
#include "ToolComponent.generated.h"

UCLASS(ClassGroup = "Storage", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UToolComponent : public UStorage
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void AddItem(UItem* ItemToAdd) override;

};
