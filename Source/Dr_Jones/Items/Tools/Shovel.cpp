// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

#include "ArchaeologicalSite/ArchaeologicalSite.h"
#include "ArchaeologicalSite/ExcavationSegment.h"
#include "Components/ShapeComponent.h"
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

bool AShovel::IsFilled() const
{
	return bFilled;
}

void AShovel::Dig()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Dig"));

	if (!OwningPlayer)
	{
		return;
	}

	FHitResult Hit;
	if (!TraceDig(Hit))
	{
		return;
	}
	
	UExcavationSegment* ExcavationSegment = Cast<UExcavationSegment>(Hit.GetComponent());
	if (!ExcavationSegment)
	{
		return;
	}
	
	DigInExcavationSite(*ExcavationSegment, Hit.ImpactPoint);
	FillShovel();
}

void AShovel::Dump()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Dump"));

	if (!OwningPlayer)
	{
		return;
	}

	const FHitResult Hit = OwningPlayer->GetPlayerLookingAt(150.0f);
	if (!Hit.bBlockingHit)
	{
		return;
	}
	
	UExcavationSegment* ExcavationSegment = Cast<UExcavationSegment>(Hit.GetComponent());
	if (!ExcavationSegment)
	{
		return;
	}
	
	DigInExcavationSite(*ExcavationSegment, Hit.ImpactPoint);
	FillShovel();
}

UShapeComponent* AShovel::GetDigCollision() const
{
	if (DigCollisionComponent)
	{
		return DigCollisionComponent;
	}
	
	const TArray<UActorComponent*> FoundComponents = GetComponentsByTag(UShapeComponent::StaticClass(), TEXT("DigCollision"));
	if (FoundComponents.IsEmpty())
	{
		return nullptr;
	}

#if !UE_BUILD_SHIPPING
	if (FoundComponents.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("More than one component tagged with DigCollision found in %s."), *this->GetName());
	}
#endif

	DigCollisionComponent = Cast<UShapeComponent>(FoundComponents[0]);
	return DigCollisionComponent;
}

void AShovel::DigInExcavationSite(UExcavationSegment& ExcavationSegment, const FVector& Location) const
{
	// TODO: Shovel shouldn't know anything about excavation segments.

	checkf(OwningPlayer, TEXT("The Shovel %s is not owned by any player."), *this->GetName());

	const FVector DigDir = FVector(0, 0, -ShovelStrength + (2 * ShovelStrength * static_cast<int>(bFilled)));

	ExcavationSegment.Dig(FTransform(OwningPlayer->GetActorRotation(), FVector(0, 0, 0) - (ExcavationSegment.GetComponentLocation() - Location), OwningPlayer->GetActorScale3D()), DigDir);
	for (UExcavationSegment* Neighbor : ExcavationSegment.Neighbors)
	{
		checkf(Neighbor, TEXT("Neighboring excavation segment is null"));
		Neighbor->Dig(FTransform(OwningPlayer->GetActorRotation(), FVector(0, 0, 0) - (Neighbor->GetComponentLocation() - Location), OwningPlayer->GetActorScale3D()), DigDir);
	}
}

bool AShovel::TraceDig(FHitResult& OutHit) const
{
	const UShapeComponent* DigCollision = GetDigCollision();
#if !UE_BUILD_SHIPPING
	if (!DigCollision)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find Dig Collision component in %s"), *this->GetName());
		return false;
	}
#else
	checkf(DigCollision, TEXT("Cannot find Dig Collision component in %s"), *this->GetName());
#endif

	const UWorld* World = GetWorld();
	checkf(World, TEXT("World is null"));

	// Sweep from Shovel actor start to the tip (dig collider)
	const FQuat Rotation = DigCollision->GetComponentRotation().Quaternion();
	const FVector& StartLocation = GetActorLocation();
	const FVector& EndLocation = DigCollision->GetComponentLocation();

	FCollisionQueryParams Params {};
	Params.AddIgnoredActor(this);

	return World->SweepSingleByChannel(OutHit, StartLocation, EndLocation, Rotation, ECC_Visibility, DigCollision->GetCollisionShape(), Params);
}

