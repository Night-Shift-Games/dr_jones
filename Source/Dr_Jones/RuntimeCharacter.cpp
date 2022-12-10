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

void ARuntimeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnActionKeyPressed.RemoveAll(this);
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
	PlayerInputComponent->BindAxis("Scroll", this, &ARuntimeCharacter::SwitchItem);
}

void ARuntimeCharacter::AddArtefact(UArtefact* Artefact)
{
	UArtefact* NewArtefact = NewObject<UArtefact>(this, Artefact->StaticClass());
	NewArtefact->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Artefacts.Add(NewArtefact);
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
	IInteractiveObject* IO = Cast<IInteractiveObject>(Hit.GetActor());
	if (!IO) 
	{
		IO = Cast<IInteractiveObject>(Hit.GetComponent());
	}
	if (!IO)
	{
		return;
	}
	

	IO->Interact(this);
}

void ARuntimeCharacter::SwitchItem(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
	ToolComponent->ScrollItem(AxisValue);
}
