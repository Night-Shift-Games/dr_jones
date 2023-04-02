// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS( ClassGroup=(Interact), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractableComponent();

	UFUNCTION(BlueprintCallable)
	void Interact();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteract InteractDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
