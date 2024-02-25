// Property of Night Shift Games, all rights reserved.

#include "CampEntity.h"

#include "Interaction/InteractableComponent.h"
#include "Utilities/Utilities.h"

ACampEntity::ACampEntity()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("MainInteractableComponent"));
}

void ACampEntity::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->AltInteractDelegate.AddDynamic(this, &ACampEntity::Grab);
}

void ACampEntity::Grab(ADrJonesCharacter* Grabber)
{
	if (!bGrabbed)
	{
		GetWorld()->GetTimerManager().SetTimer(GrabTimer,this, &ACampEntity::HoldActor, GetWorld()->GetDeltaSeconds() / 2.0, true);
		SetActorEnableCollision(false);
		GetRootComponent()->SetMobility(EComponentMobility::Movable);
		bGrabbed = true;
		InteractableComponent->bInteractionInProgress = true;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(GrabTimer);
		SetActorEnableCollision(true);
		bGrabbed = false;
		InteractableComponent->bInteractionInProgress = false;
	}
}

void ACampEntity::HoldActor()
{
	const ADrJonesCharacter& Player = Utilities::GetPlayerCharacter(*this);

	const FHitResult PlayerHitResult = Utilities::GetPlayerLookingAt(200.f);
	const FVector FoundLocation = Utilities::FindGround(*this, PlayerHitResult.Location + FVector(0.0, 0.0, 20.0), { this });
	SetActorLocation(FoundLocation, false);

	SetActorRotation((Player.GetActorForwardVector() * -1).Rotation());
}

