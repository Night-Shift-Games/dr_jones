// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DrJonesCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class AItem;
class UBlendCameraComponent;
class UCharacterAnimationComponent;
class UInputAction;
class UInteractionComponent;
class UEquipmentComponent;
class UReactionComponent;
class UReputationComponent;
class UWidgetManager;
struct FInputActionValue;

UCLASS(Blueprintable)
class DR_JONES_API ADrJonesCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ADrJonesCharacter();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UWidgetManager* GetWidgetManager() const { return WidgetManager; }
	UEquipmentComponent* GetInventory() const { return EquipmentComponent; }
	UBlendCameraComponent* GetCameraBlend() const { return BlendCameraComponent; }

private:
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetManager> WidgetManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UReputationComponent> ReputationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBlendCameraComponent> BlendCameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterAnimationComponent> CharacterAnimationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
};
