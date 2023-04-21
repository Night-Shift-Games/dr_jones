// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

#include "ArchaeologicalSite/ArchaeologicalSite.h"
#include "ArchaeologicalSite/ExcavationSegment.h"
#include "Kismet/GameplayStatics.h"

void AShovel::PlayFX(const FHitResult& Hit)
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
		DirtComponent = NewObject<UStaticMeshComponent>(GetRootComponent(), UStaticMeshComponent::StaticClass());
		DirtComponent->RegisterComponent();
		DirtComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
		DirtComponent->SetStaticMesh(ShovelDirt);
		DirtComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (DirtComponent)
	{
		DirtComponent->DestroyComponent();
	}
}

void AShovel::Dig()
{
	Player = Cast<ADrJonesCharacter>(UGameplayStatics::GetPlayerCharacter(GWorld, 0));
	const FHitResult Hit = Player->GetPlayerLookingAt(ShovelReach);
	if (UExcavationSegment* ExcavationSite = Cast<UExcavationSegment>(Hit.GetComponent()))
	{
		// TODO: Shovel shouldn't know anything about terrain.

		const FVector DigDir = FVector(0, 0, -ShovelStrength + (2 * ShovelStrength * static_cast<int>(bFilled)));

		ExcavationSite->Dig(FTransform(Player->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSite->GetComponentLocation() - Hit.Location), Player->GetActorScale3D()), DigDir);
		for (UExcavationSegment* x : ExcavationSite->Neighbors)
		{
			x->Dig(FTransform(Player->GetActorRotation(), FVector(0, 0, 0) - (x->GetComponentLocation() - Hit.Location), Player->GetActorScale3D()), DigDir);
		}
		FillShovel();
		PlayFX(Hit);
	}
}

