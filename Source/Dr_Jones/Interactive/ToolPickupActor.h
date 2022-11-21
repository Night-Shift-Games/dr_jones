// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"
#include "../Items/Tools/Tool.h"
#include ".././RuntimeCharacter.h"
#include "ToolPickupActor.generated.h"

UCLASS()
class DR_JONES_API AToolPickupActor : public AInteractiveActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTool> Tool;

	virtual void Interact(APawn* Indicator) override;
	
};
