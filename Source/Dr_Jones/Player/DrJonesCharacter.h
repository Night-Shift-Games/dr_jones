// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DrJonesCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInteractionComponent;

UCLASS(Blueprintable)
class DR_JONES_API ADrJonesCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ADrJonesCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	static void DrawInteractionDebugInfo(FVector WorldLocation, FVector LineEnd, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	static FHitResult GetPlayerLookingAt(const float Reach);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Input Component
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);

	// Interaction Component
	void PrimaryAction();
	void SecondaryAction();

	// Action Component
	void Interact();
	void SwitchItem(float AxisValue);
public:
	// Widget Component
	void ShowInteractionUI();
	void HideInteractionUI();

public:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	// TODO: InputComponent;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnPrimaryActionKeyPressed;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnSecondaryActionKeyPressed;

	// TODO: AudioComponent
	UPROPERTY(EditAnywhere, Category = "FX|Sound")
	TObjectPtr<USoundBase> DigSound;

	// TODO: AnimationComponent
	UPROPERTY(EditAnywhere, Category = "FX|Anim")
	TObjectPtr<UAnimMontage> DigAnim;


	// TODO: WidgetComponent
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> InteractionWidgetUIClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> InteractionWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> RunTimeUIClass;

};
