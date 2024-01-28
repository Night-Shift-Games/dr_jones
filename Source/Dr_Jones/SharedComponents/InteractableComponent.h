// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Player/DrJonesCharacter.h"

#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, ADrJonesCharacter*, Player);

UCLASS( ClassGroup = "Shared Components", meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	UInteractableComponent();

	UFUNCTION(BlueprintCallable)
	void Interact(ADrJonesCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void AltInteract(ADrJonesCharacter* Player);

	void SetInteractionEnabled(bool bEnabled) { bEnabledInteraction = bEnabled; }
	bool IsInteractionEnabled() const { return bEnabledInteraction; }

	UMeshComponent* GetOwnerMesh() const;
	void SetRenderPostProcessInteractionOutline(bool bRender) const;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteract InteractDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteract AltInteractDelegate;
	
private:
	bool bEnabledInteraction = true;
	bool bIsActorInteraction = false;
};
