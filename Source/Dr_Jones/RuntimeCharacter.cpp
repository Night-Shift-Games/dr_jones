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

	Viewtarget = GetPlayerLookingAt(300);
	if (auto x = AsInteractive(Viewtarget))
	{
		FString Sentence = x->GetInteractSentence();
		AddPickupWidget(Sentence);
	}
	else
	{
		RemovePickupWidget();
	}
}

TArray<UArtefact*> ARuntimeCharacter::GetArtefacts()
{
	return Artefacts;
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
	NewArtefact->SetParameters(Artefact);
	NewArtefact->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Artefacts.Add(NewArtefact);
}

FHitResult ARuntimeCharacter::GetPlayerLookingAt(const float Reach)
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	GetController()->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * Reach;

	GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility);

	return Hit;
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
	if (IInteractiveObject* IO = AsInteractive(Viewtarget))
	{
		IO->Interact(this);
	}
}

void ARuntimeCharacter::SwitchItem(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
	ToolComponent->ScrollItem(AxisValue);
}

IInteractiveObject* ARuntimeCharacter::AsInteractive(FHitResult Interactive)
{
	IInteractiveObject* IO = Cast<IInteractiveObject>(Interactive.GetActor());
	if (!IO)
	{
		IO = Cast<IInteractiveObject>(Interactive.GetComponent());
	}
	if (!IO)
	{
		return nullptr;
	}
	return IO;
}
