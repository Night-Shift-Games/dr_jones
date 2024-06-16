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
	InteractableComponent->AltInteractDelegate.AddDynamic(this, &ACampEntity::ToggleGrab);
}

void ACampEntity::ToggleGrab(ADrJonesCharacter* Grabber)
{
	if (!bGrabbed)
	{
		Grab();
	}
	else
	{
		Ungrab();
	}
}

void ACampEntity::SetGrabPostProcessEnabled(bool bEnabled)
{
	const int32 CustomStencilID = bEnabled ? 2 : 0;
	
	for (const AActor* Actor : ActorsToIgnoreDuringGrab)
	{
		TArray<UMeshComponent*> MeshComponents;
		Actor->GetComponents<UMeshComponent>(MeshComponents, true);
		for (UMeshComponent* Mesh : MeshComponents)
		{
			Mesh->SetCustomDepthStencilValue(CustomStencilID);
			Mesh->SetRenderCustomDepth(bEnabled);
		}
	}
}

void ACampEntity::Grab()
{
	SetActorEnableCollision(false);
	GetRootComponent()->SetMobility(EComponentMobility::Movable);
	bGrabbed = true;
	InteractableComponent->bInteractionInProgress = true;
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true);
	AttachedActors.Add(this);
	ActorsToIgnoreDuringGrab.Reset();
	ActorsToIgnoreDuringGrab.Append(AttachedActors);

	SetGrabPostProcessEnabled(true);
	
	GetWorld()->GetTimerManager().SetTimer(GrabTimer,this, &ACampEntity::UpdateActorPosition, GetWorld()->GetDeltaSeconds(), true);
}

void ACampEntity::Ungrab()
{
	GetWorld()->GetTimerManager().ClearTimer(GrabTimer);
	SetActorEnableCollision(true);
	bGrabbed = false;
	InteractableComponent->bInteractionInProgress = false;

	SetGrabPostProcessEnabled(false);
	
	if (AActor* ActorToAttachTo = GrabResult.GetActor())
	{
		AttachToActor(ActorToAttachTo, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void ACampEntity::UpdateActorPosition()
{
	const ADrJonesCharacter& Player = Utilities::GetPlayerCharacter(*this);
	const FHitResult PlayerHitResult = Utilities::GetPlayerSightTarget(200.f, *this);
	
	GrabResult = Utilities::FindGround(*this, PlayerHitResult.Location + FVector(0.0, 0.0, 20.0), ActorsToIgnoreDuringGrab);

	if (!GrabResult.bBlockingHit)
	{
		return;
	}
	
	const FVector FoundLocation = GrabResult.Location;
	
	SetActorLocation(FoundLocation);
	SetActorRotation((Player.GetActorForwardVector() * -1).Rotation());
}
