// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "PlayerComponents/InteractionComponent.h"

ADrJonesCharacter::ADrJonesCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void ADrJonesCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(RunTimeUIClass))
	{
		UUserWidget* UI = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), RunTimeUIClass, TEXT("Runtime"));

		if (IsValid(UI))
		{
			UI->AddToViewport();
		}
	}
}

void ADrJonesCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnPrimaryActionKeyPressed.RemoveAll(this);
}

void ADrJonesCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ADrJonesCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADrJonesCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ADrJonesCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADrJonesCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryItemAction", IE_Pressed, this, &ADrJonesCharacter::PrimaryAction);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADrJonesCharacter::Interact);
	PlayerInputComponent->BindAxis("Scroll", this, &ADrJonesCharacter::SwitchItem);
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
	DrawDebugLine(GWorld, WorldLocation, LineEnd, FColor::Red, false, 1.f / 60, 0, 1);

	return Hit;
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

void ADrJonesCharacter::PrimaryAction()
{
	OnPrimaryActionKeyPressed.Broadcast();
}

void ADrJonesCharacter::SecondaryAction()
{
	OnSecondaryActionKeyPressed.Broadcast();
}

void ADrJonesCharacter::Interact()
{
	checkf(InteractionComponent, TEXT("InteractionComponent is missing!"));
	InteractionComponent->Interact();
}

void ADrJonesCharacter::SwitchItem(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
//	ToolInventory->ScrollItem(AxisValue);
}

void ADrJonesCharacter::ShowInteractionUI()
{
	if (IsValid(InteractionWidgetUIClass))
	{
		InteractionWidget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), InteractionWidgetUIClass, TEXT("Interaction"));

		if (IsValid(InteractionWidget))
		{
			InteractionWidget->AddToViewport();
		}
	}
}

void ADrJonesCharacter::HideInteractionUI()
{
	if (InteractionWidget && InteractionWidget->IsInViewport())
	{
		InteractionWidget->RemoveFromParent();
	}
}
