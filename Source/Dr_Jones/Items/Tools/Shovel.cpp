// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"
#include "ArchaeologicalSite/ExcavationSegment.h"
#include "Kismet/GameplayStatics.h"

void AShovel::UseToolPrimaryAction()
{
	if (!PlayerReference)
	{
		Player = Cast<ADrJonesCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
	Dig();
}

void AShovel::PlayFX(FHitResult Hit)
{
	if (USoundBase* SoundDig = Player->DigSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GWorld, SoundDig, Hit.Location);
	}
	if (UAnimMontage* DigAnim = Player->DigAnim)
	{
		Player->PlayAnimMontage(DigAnim);
	}
}

void AShovel::FillShovel()
{
	bFilled = !bFilled;
	if (bFilled)
	{
		/*DirtComponent = NewObject<UStaticMeshComponent>(PlayerRefrence, UStaticMeshComponent::StaticClass());
		DirtComponent->RegisterComponent();
		DirtComponent->AttachToComponent(PlayerRefrence, FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
		DirtComponent->SetStaticMesh(ShovelDirt);
		DirtComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	}
	else
	{
		if (DirtComponent)
		{
			DirtComponent->DestroyComponent();
		}
	}
}

FHitResult AShovel::GetHit()
{
	FVector OUT ControllerViewportLocation;
	FRotator OUT ControllerViewportRotation;

	Player->GetActorEyesViewPoint(ControllerViewportLocation, ControllerViewportRotation);

	FHitResult OUT Hit;

	FVector LineEnd = ControllerViewportLocation + ControllerViewportRotation.Vector() * 350;

	GWorld->LineTraceSingleByChannel(Hit, ControllerViewportLocation, LineEnd, ECollisionChannel::ECC_Visibility);

	return Hit;
}

void AShovel::Dig()
{
	FHitResult Hit = GetHit();
	if (UExcavationSegment* ExcavationSite = Cast<UExcavationSegment>(Hit.GetComponent()))
	{
		// TODO: Shovel shouldn't know anything about terrain.

		FVector DigDir = FVector(0, 0, -ShovelStrength + (2 * ShovelStrength * static_cast<int>(bFilled)));
		
		ExcavationSite->Dig(FTransform(Player->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSite->GetComponentLocation() - Hit.Location), Player->GetActorScale3D()), DigDir);
		for (UExcavationSegment* x : ExcavationSite->Neighbors)
		{
			x->Dig(FTransform(Player->GetActorRotation(), FVector(0, 0, 0) - (x->GetComponentLocation() - Hit.Location), Player->GetActorScale3D()), DigDir);
		}
		FillShovel();
		PlayFX(Hit);
	}
}

