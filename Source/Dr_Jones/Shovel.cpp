// Property of Night Shift Games, all rights reserved.


#include "Shovel.h"

void UShovel::BeginPlay()
{
	Cast<ARuntimeCharacter>(GetOwner())->OnActionKeyPressed.AddDynamic(this, &UShovel::Dig);
}

void UShovel::Dig()
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	Cast<APawn>(GetOwner())->GetController()->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * 500;

	if (!GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility)) return;
	//DrawDebugLine(GWorld, ControllerViewportLocation, Hit.Location, FColor::Green, false, 5);
	if (UExcavationSegment* ExcavationSite = Cast<UExcavationSegment>(Hit.GetComponent()))
	{
		//DrawDebugBox(GWorld, Hit.Location, FVector(30, 30, 30), FColor::Green, false, 5);
		ExcavationSite->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSite->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()));
		for (UExcavationSegment* x : ExcavationSite->Neighbors)
		{
			x->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (x->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()));
		}
	}
}
