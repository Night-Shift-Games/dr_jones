// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "InteractiveActor.generated.h"

UCLASS()
class DR_JONES_API AInteractiveActor : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AInteractiveActor();

	virtual void Interact(APawn* Indicator) override;

protected:
	UStaticMeshComponent* StaticMeshComponent;

};