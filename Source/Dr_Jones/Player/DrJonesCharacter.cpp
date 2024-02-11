// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "Animation/CharacterAnimationComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "PlayerComponents/InteractionComponent.h"
#include "PlayerComponents/EquipmentComponent.h"
#include "PlayerComponents/ReactionComponent.h"
#include "PlayerComponents/ReputationComponent.h"
#include "SharedComponents/BlendCameraComponent.h"
#include "WidgetManager.h"

ADrJonesCharacter::ADrJonesCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	ReactionComponent = CreateDefaultSubobject<UReactionComponent>(TEXT("ReactionComponent"));
	InventoryComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Inventory"));
	WidgetManager = CreateDefaultSubobject<UWidgetManager>(TEXT("WidgetManager"));
	ReputationComponent = CreateDefaultSubobject<UReputationComponent>(TEXT("ReputationComponent"));
	CharacterAnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("CharacterAnimationComponent"));
	BlendCameraComponent = CreateDefaultSubobject<UBlendCameraComponent>(TEXT("BlendCameraComponent"));
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	
	InteractionComponent->SetupPlayerInput(EnhancedInputComponent);
	InventoryComponent->SetupPlayerInput(EnhancedInputComponent);
	ReactionComponent->SetupPlayerInput(EnhancedInputComponent);
}

void ADrJonesCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Vector = InputActionValue.Get<FVector2D>();
	if (Vector.X != 0)
	{
		AddMovementInput(GetActorForwardVector(), Vector.X);
	}
	if (Vector.Y != 0)
	{
		AddMovementInput(GetActorRightVector(), Vector.Y);
	}
}

void ADrJonesCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D Vector = InputActionValue.Get<FVector2D>();
	if (Vector.Length() == 0)
	{
		return;
	}
	AddControllerYawInput(Vector.X);
	AddControllerPitchInput(Vector.Y);
}

