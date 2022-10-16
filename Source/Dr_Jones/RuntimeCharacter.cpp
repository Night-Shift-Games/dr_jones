// Property of Night Shift Games, all rights reserved.


#include "RuntimeCharacter.h"

// Sets default values
ARuntimeCharacter::ARuntimeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARuntimeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARuntimeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARuntimeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ARuntimeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARuntimeCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ARuntimeCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ARuntimeCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
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
