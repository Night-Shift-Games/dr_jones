// Property of Night Shift Games, all rights reserved.

#include "Shovel.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Components/ShapeComponent.h"
#include "Items/ActionComponent.h"
#include "Player/DrJonesCharacter.h"
#include "Utilities/Utilities.h"

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
		UMeshComponent* MainMesh = FindComponentByClass<UMeshComponent>();
		DirtComponent->AttachToComponent(MainMesh ? MainMesh : GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName("ShovelInput"));
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
	AnimationComponent.PlayItemMontage(Montage);
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
	return DirtComponent != nullptr;
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
}

void AShovel::Dump()
{
	GetWorldTimerManager().ClearTimer(ActionComponent->AnimationTimer);
	if (!OwningPlayer)
	{
		return;
	}

	const FHitResult Hit = Utilities::GetPlayerSightTarget(150.0f, *this);
	if (!Hit.bBlockingHit)
	{
		return;
	}
}

void AShovel::PrimaryAction()
{
	FTimerHandle& Handle = ActionComponent->AnimationTimer;
	
	if (IsFilled())
	{
		Dump();
	}
	else
	{
		Dig();
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
	OutHit = Utilities::GetPlayerSightTarget(ShovelReach, *this);
	return OutHit.bBlockingHit;
}

