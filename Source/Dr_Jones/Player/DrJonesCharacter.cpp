// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComponents/InventoryComponent.h"
#include "PlayerComponents/InteractionComponent.h"
#include "PlayerComponents/ReactionComponent.h"
#include "PlayerComponents/ReputationComponent.h"
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
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ADrJonesCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADrJonesCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ADrJonesCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADrJonesCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InventoryComponent->SetupPlayerInput(PlayerInputComponent);
	ReactionComponent->SetupPlayerInput(PlayerInputComponent);
	InteractionComponent->SetupPlayerInput(PlayerInputComponent);
}

void ADrJonesCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue, false);
}

void ADrJonesCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue, false);
}

void ADrJonesCharacter::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ADrJonesCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

/*static*/ FHitResult ADrJonesCharacter::GetPlayerLookingAt(const float Reach)
{
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(GWorld, 0);

	int32 ViewportX, ViewportY;
	Controller->GetViewportSize(ViewportX, ViewportY);
	FVector2D ScreenCenter = FVector2D(ViewportX / 2, ViewportY / 2);

	FVector WorldLocation, WorldDirection;
	if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return {};
	}

	FVector LineEnd = WorldLocation + WorldDirection * Reach;
	FHitResult Hit;
	
	GWorld->LineTraceSingleByChannel(Hit, WorldLocation, LineEnd, ECollisionChannel::ECC_Visibility);
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