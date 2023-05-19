// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

#include "Animation/CharacterAnimationComponent.h"
#include "ArchaeologicalSite/ExcavationSegment.h"
#include "Components/ShapeComponent.h"
#include "Player/DrJonesCharacter.h"
#include "Player/PlayerComponents/HotBarComponent.h"
#include "SharedComponents/ActionComponent.h"

void AShovel::BeginPlay()
{
	Super::BeginPlay();

	checkf(ActionComponent, TEXT("Action Component is not set."));
	ActionComponent->PrimaryActionDelegate.AddDynamic(this, &AShovel::PrimaryAction);
}

void AShovel::FillShovel()
{
	if (!IsFilled())
	{
		DirtComponent = NewObject<UStaticMeshComponent>(GetRootComponent(), UStaticMeshComponent::StaticClass());
		DirtComponent->RegisterComponent();
		DirtComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
		DirtComponent->SetStaticMesh(ShovelDirt);
		DirtComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AShovel::EmptyShovel()
{
	if (IsFilled())
	{
		DirtComponent->DestroyComponent();
		DirtComponent = nullptr;
	}
}

void AShovel::PrimaryActionMontageBehavior()
{
	checkf(OwningPlayer->CharacterAnimationComponent, TEXT("Character has no animation component"));
	UCharacterAnimationComponent& AnimationComponent = *OwningPlayer->CharacterAnimationComponent;
	
	UAnimMontage* Montage = nullptr;
	
	FHitResult DigLocationHit;
	if (TraceForDesiredDigLocation(DigLocationHit))
	{
		AnimationComponent.ItemIKData.IKTrackLocation = DigLocationHit.Location;
#if ENABLE_DRAW_DEBUG
		if (CVarToolDebug.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), AnimationComponent.ItemIKData.IKTrackLocation, 10.0f, 16, FColor::Green, false, 2.0f);
		}
#endif
		Montage = FindActionMontage(IsFilled() ? TEXT("Dump") : TEXT("Dig"));
	}
	else if (IsFilled())
	{
		Montage = FindActionMontage(TEXT("Drop"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation Montage not set."));
		return;
	}
	
#if ENABLE_DRAW_DEBUG
	if (CVarToolDebug.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("PrimaryAction: %s"), *Montage->GetName()));
	}
#endif
	AnimationComponent.PlayMontage(Montage);
	AnimationComponent.OnMontageCompleted.AddUniqueDynamic(this, &AShovel::MontageCompletedEvent);
}

UShapeComponent* AShovel::GetDigCollision() const
{
// TODO:
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

bool AShovel::IsFilled() const
{
	return !DirtComponent.IsNull();
}

void AShovel::Dig()
{
	GetWorldTimerManager().ClearTimer(ActionComponent->AnimationTimer);
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
	GetWorldTimerManager().ClearTimer(ActionComponent->AnimationTimer);
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
	EmptyShovel();
}

void AShovel::PrimaryAction()
{
	FTimerHandle& Handle = ActionComponent->AnimationTimer;

	if (Handle.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(10, 2, FColor::Red, __func__);
		return;
	}
	PrimaryActionMontageBehavior();
	if (IsFilled())
	{
		GetWorldTimerManager().SetTimer(Handle, this, &AShovel::Dump, 1, false, 1);
	}
	else
	{
		GetWorldTimerManager().SetTimer(Handle, this, &AShovel::Dig, 1, false, 1);
	}
}

void AShovel::MontageCompletedEvent(bool bInterrupted)
{
	if (!OwningPlayer)
	{
		return;
	}
	OwningPlayer->CharacterAnimationComponent->OnMontageCompleted.RemoveDynamic(this, &AShovel::MontageCompletedEvent);
}

void AShovel::DigInExcavationSite(UExcavationSegment& ExcavationSegment, const FVector& Location) const
{
	// TODO: Shovel shouldn't know anything about excavation segments.

	checkf(OwningPlayer, TEXT("The Shovel %s is not owned by any player."), *this->GetName());

	const FVector DigDir = FVector(0, 0, -ShovelStrength + (2 * ShovelStrength * static_cast<int>(IsFilled())));

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

bool AShovel::TraceForDesiredDigLocation(FHitResult& OutHit) const
{
	OutHit = OwningPlayer->GetPlayerLookingAt(ShovelReach);
	return OutHit.bBlockingHit;
}

