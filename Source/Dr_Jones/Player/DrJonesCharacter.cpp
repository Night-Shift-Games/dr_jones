// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "Animation/CharacterAnimationComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComponents/InteractionComponent.h"
#include "PlayerComponents/InventoryComponent.h"
#include "PlayerComponents/ReactionComponent.h"
#include "PlayerComponents/ReputationComponent.h"
#include "SharedComponents/BlendCameraComponent.h"
#include "WidgetManager.h"

ADrJonesCharacter::ADrJonesCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	ReactionComponent = CreateDefaultSubobject<UReactionComponent>(TEXT("ReactionComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	WidgetManager = CreateDefaultSubobject<UWidgetManager>(TEXT("WidgetManager"));
	ReputationComponent = CreateDefaultSubobject<UReputationComponent>(TEXT("ReputationComponent"));
	CharacterAnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("CharacterAnimationComponent"));
	BlendCameraComponent = CreateDefaultSubobject<UBlendCameraComponent>(TEXT("BlendCameraComponent"));
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	

	InventoryComponent->SetupPlayerInput(PlayerInputComponent);
	ReactionComponent->SetupPlayerInput(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	
	InteractionComponent->SetupPlayerInput(EnhancedInputComponent);
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

/*static*/ FHitResult ADrJonesCharacter::GetPlayerLookingAt(const float Reach)
{
	// TODO: Add WCO
	UWorld* World = GWorld;
	if (!World)
	{
		return {};
	}
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);

	int32 ViewportX, ViewportY;
	Controller->GetViewportSize(ViewportX, ViewportY);
	FVector2D ScreenCenter = FVector2D(ViewportX / 2.0, ViewportY / 2.0);

	FVector WorldLocation, WorldDirection;
	if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return {};
	}

	FVector LineEnd = WorldLocation + WorldDirection * Reach;
	FHitResult Hit;
	
	World->LineTraceSingleByChannel(Hit, WorldLocation, LineEnd, ECC_Visibility);

	if (!Hit.bBlockingHit)
	{
		Hit.Location = LineEnd;
	}
#if ENABLE_DRAW_DEBUG
	DrawInteractionDebugInfo(WorldLocation, LineEnd, Hit);
#endif
	
	return Hit;
}

#if ENABLE_DRAW_DEBUG
void ADrJonesCharacter::DrawInteractionDebugInfo(const FVector& WorldLocation, const FVector& LineEnd, const FHitResult& Hit)
{
	if (CVarInteraction.GetValueOnAnyThread() == 0)
	{
		return;
	}
	DrawDebugLine(GWorld, WorldLocation, LineEnd, FColor::Red, false, 0, 0, 1);
	FString Debug = FString(TEXT("Pointed Actor: "));
	if (const AActor* Actor = Hit.GetActor())
	{
		Debug += Actor->GetName();
	}
	GEngine->AddOnScreenDebugMessage(420, 10, FColor::Green, Debug);
}
#endif