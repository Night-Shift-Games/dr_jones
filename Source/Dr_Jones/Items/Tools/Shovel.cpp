// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

void UShovel::UseTool()
{
	if (!PlayerRefrence)
	{
		PlayerRefrence = GetOwner<ARuntimeCharacter>()->ToolComponent->GetHand();
	}
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
			DirtComponent = NewObject<UStaticMeshComponent>(PlayerRefrence, UStaticMeshComponent::StaticClass());
			DirtComponent->RegisterComponent();
			DirtComponent->AttachToComponent(PlayerRefrence, FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
			DirtComponent->SetStaticMesh(ShovelDirt);
			DirtComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			if (DirtComponent)
			{
				DirtComponent->DestroyComponent();
			}
		}
	}
}

