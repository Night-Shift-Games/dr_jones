// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DrJonesCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInteractionComponent;
class UReactionComponent;
class UHotBarComponent;
class UCharacterAnimationComponent;
class UWidgetManager;
class AItem;

UCLASS(Blueprintable)
class DR_JONES_API ADrJonesCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ADrJonesCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UWidgetManager* GetWidgetManager() const { return WidgetManager; };
	static void DrawInteractionDebugInfo(const FVector& WorldLocation, const FVector& LineEnd, const FHitResult& Hit);

	UFUNCTION(BlueprintPure)
	static FHitResult GetPlayerLookingAt(const float Reach);

private:
	// Input Component
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);

	// Action Component
	void Interact();
	
	void SwitchItem(float AxisValue);

public:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UReactionComponent> ReactionComponent;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UWidgetManager> WidgetManager;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UHotBarComponent> HotBarComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterAnimationComponent> CharacterAnimationComponent;

	// TODO: AnimationComponent
	UPROPERTY(EditAnywhere, Category = "FX|Sound")
	TObjectPtr<USoundBase> DigSound;
	
	UPROPERTY(EditAnywhere, Category = "FX|Anim")
	TObjectPtr<UAnimMontage> DigAnim;
};
