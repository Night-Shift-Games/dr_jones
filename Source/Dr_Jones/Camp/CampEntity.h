// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CampEntity.generated.h"

class UInteractableComponent;

UCLASS()
class DR_JONES_API ACampEntity : public AActor
{
	GENERATED_BODY()

public:
	ACampEntity();
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInteractableComponent> InteractableComponent;
};
