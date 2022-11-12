// Property of Night Shift Games, all rights reserved.


#include "RuntimeCharacter.h"

ARuntimeCharacter::ARuntimeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	ToolComponent = CreateDefaultSubobject<UToolComponent>(TEXT("ToolInventory"));
}

void ARuntimeCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARuntimeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARuntimeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ARuntimeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARuntimeCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ARuntimeCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ARuntimeCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryItemAction", IE_Pressed, this, &ARuntimeCharacter::PrimaryAction);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARuntimeCharacter::Interact);
}

void ARuntimeCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue, false);
}

void ARuntimeCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue, false);
}

void ARuntimeCharacter::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ARuntimeCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void ARuntimeCharacter::PrimaryAction()
{
	OnActionKeyPressed.Broadcast();
}

void ARuntimeCharacter::Interact()
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	GetController()->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * 500;

	if (!GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility))
	{
		return;
	}
	if (IInteractiveObject* IO = Cast<IInteractiveObject>(Hit.GetActor()))
	{
		IO->Interact(this);
	}
}
