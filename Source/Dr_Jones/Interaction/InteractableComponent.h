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
	bool Interact(ADrJonesCharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool AltInteract(ADrJonesCharacter* Player);

	void SetInteractionEnabled(bool bEnabled) { bEnabledInteraction = bEnabled; }
	bool IsInteractionEnabled() const { return bEnabledInteraction; }

	UMeshComponent* GetOwnerMesh() const;
	void SetRenderPostProcessInteractionOutline(bool bRender) const;
	bool IsInteractionInProgress() const { return bInteractionInProgress; }

public:
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Interaction")
	FOnInteract InteractDelegate;

	UPROPERTY(BlueprintAssignable, Category = "DrJones|Interaction")
	FOnInteract AltInteractDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText InteractionText = FText::FromString("Interact");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText AltInteractionText = FText::FromString("Alt Interact");
	
	bool bInteractionInProgress = false;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bEnabledInteraction = true;
	
};
