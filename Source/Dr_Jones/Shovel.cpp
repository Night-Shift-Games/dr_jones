// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

void UShovel::BeginPlay()
{
	Super::BeginPlay();
	bFilled = false;
}

void UShovel::Dig()
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	Cast<APawn>(GetOwner())->GetController()->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * 500;

	if (!GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility)) return;
	if (UExcavationSegment* ExcavationSite = Cast<UExcavationSegment>(Hit.GetComponent()))
	{
		FVector DigDir = FVector(0, 0, -10 + (20 * (int)bFilled));
		
		ExcavationSite->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSite->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()), DigDir);
		for (UExcavationSegment* x : ExcavationSite->Neighbors)
		{
			x->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (x->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()), DigDir);
		}
		bFilled = !bFilled;
	}

}

void UShovel::SetupTool()
{
	ARuntimeCharacter* Character = Cast<ARuntimeCharacter>(GetOwner());
	Character->OnActionKeyPressed.AddDynamic(this, &UShovel::Dig);
}
