// Property of Night Shift Games, all rights reserved.

#include "ArchaeologistDesk.h"

#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Quest/QuestMessages.h"
#include "Utilities/Utilities.h"
#include "World/Illuminati.h"

AArchaeologistDesk::AArchaeologistDesk()
{
	DeskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = DeskMesh;
	InteractableComponent->SetupAttachment(DeskMesh);
}

void AArchaeologistDesk::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->InteractDelegate.AddUniqueDynamic(this, &AArchaeologistDesk::OnInteract);
}

void AArchaeologistDesk::AddArtifact(AArtifact* Artifact, ADrJonesCharacter* Player)
{
	if (!Artifact)
	{
		return;
	}
	ArtifactOnDesk = Artifact;
	Player->GetEquipment()->TakeOutItemInHand();

	FVector SocketPlace = DeskMesh->GetSocketLocation(TEXT("ArtifactSocket"));
	const double ZOffset = Utilities::GetMeshZOffset(*ArtifactOnDesk);
	SocketPlace = Utilities::FindGround(*this, SocketPlace + FVector (0, 0, 20), {ArtifactOnDesk});
	ArtifactOnDesk->AttachToComponent(DeskMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ArtifactSocket"));
	ArtifactOnDesk->SetActorLocation(SocketPlace - FVector(0,0, ZOffset));
	ArtifactOnDesk->SetOwner(this);
	
	ArtifactOnDesk->OnArtifactPickup.BindUObject(this, &AArchaeologistDesk::RemoveArtifact);
}

void AArchaeologistDesk::RemoveArtifact(AArtifact* Artifact)
{
	ArtifactOnDesk->OnArtifactPickup.Unbind();
	ArtifactOnDesk = nullptr;
}

void AArchaeologistDesk::OnInteract(ADrJonesCharacter* Player)
{
	// It should depend which part of the desk is clicked - Components?
	const UEquipmentComponent* EquipmentComponent = Player->GetEquipment();
	AItem* ItemInHand = EquipmentComponent->GetItemInHand();
	
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr; Artifact && !ArtifactOnDesk)
	{
		AddArtifact(Artifact, Player);
	}
}
