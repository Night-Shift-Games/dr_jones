// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

UShovel::UShovel()
{
	DirtComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dirt"));
	DirtComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
	DirtComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UShovel::UseTool()
{
	Dig();
}

void UShovel::Dig()
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	Cast<APawn>(GetOwner())->GetController()->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * 350;

	if (!GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility))
	{
		return;
	}
	if (UExcavationSegment* ExcavationSite = Cast<UExcavationSegment>(Hit.GetComponent()))
	{
		FVector DigDir = FVector(0, 0, -ShovelStrengh + (2 * ShovelStrengh * (int)bFilled));
		
		ExcavationSite->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSite->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()), DigDir);
		for (UExcavationSegment* x : ExcavationSite->Neighbors)
		{
			x->Dig(FTransform(GetOwner()->GetActorRotation(), FVector(0, 0, 0) - (x->GetComponentLocation() - Hit.Location), GetOwner()->GetActorScale3D()), DigDir);
		}
		bFilled = !bFilled;
		if (bFilled)
		{
			if (ShovelDirt)
			{
				DirtComponent->SetStaticMesh(ShovelDirt);
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15, FColor::Red, TEXT("Missing Tool Component"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15, FColor::Red, TEXT("dsmponent"));
		}
	}
}

