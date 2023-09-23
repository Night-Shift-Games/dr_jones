// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DrJonesCharacter.generated.h"

class UReputationComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class AItem;
class UInteractionComponent;
class UReactionComponent;
class UHotBarComponent;
class UCharacterAnimationComponent;
class UWidgetManager;

UCLASS(Blueprintable)
class DR_JONES_API ADrJonesCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ADrJonesCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UWidgetManager* GetWidgetManager() const { return WidgetManager; };

	UFUNCTION(BlueprintPure)
	static FHitResult GetPlayerLookingAt(const float Reach);

	static void DrawInteractionDebugInfo(const FVector& WorldLocation, const FVector& LineEnd, const FHitResult& Hit);

private:
	// Input Component
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UReactionComponent> ReactionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetManager> WidgetManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHotBarComponent> HotBarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UReputationComponent> ReputationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterAnimationComponent> CharacterAnimationComponent;

	// TODO: AnimationComponent
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|Sound")
	TObjectPtr<USoundBase> DigSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|Anim")
	TObjectPtr<UAnimMontage> DigAnim;
};
