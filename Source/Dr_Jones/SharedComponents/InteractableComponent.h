// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Player/DrJonesCharacter.h"

#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, ADrJonesCharacter*, Player);

UCLASS( ClassGroup = "Shared Components", meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UInteractableComponent();

	UFUNCTION(BlueprintCallable)
	void Interact(ADrJonesCharacter* Player);

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteract InteractDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
