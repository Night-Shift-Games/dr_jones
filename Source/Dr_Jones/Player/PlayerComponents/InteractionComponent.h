// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractionComponent.generated.h"

class ADrJonesCharacter;

UCLASS(ClassGroup = "PlayerComponents", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UInteractionComponent();
	void Interact();
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 150;
	TWeakObjectPtr<ADrJonesCharacter> Owner;
	TObjectPtr<AActor> ActorToInteract;
};
