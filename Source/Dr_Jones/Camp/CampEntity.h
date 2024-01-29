// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CampEntity.generated.h"

class ADrJonesCharacter;
class UInteractableComponent;

UCLASS(HideCategories = (Mobile))
class DR_JONES_API ACampEntity : public AActor
{
	GENERATED_BODY()

public:
	ACampEntity();
	virtual void BeginPlay() override;

	UFUNCTION(Blueprintable)
	void Grab(ADrJonesCharacter* Grabber);

	void HoldActor();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Interaction", meta = (NoResetToDefault))
	TObjectPtr<UInteractableComponent> InteractableComponent;

	FTimerHandle GrabTimer;
	bool bGrabbed = false;
};
