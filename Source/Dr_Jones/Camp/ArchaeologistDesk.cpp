// Property of Night Shift Games, all rights reserved.

#include "ArchaeologistDesk.h"

#include "Player/PlayerComponents/EquipmentComponent.h"
#include "Utilities.h"
#include "World/Illuminati.h"

AArchaeologistDesk::AArchaeologistDesk() : Super()
{
	DeskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = DeskMesh;
	InteractableComponent->SetupAttachment(DeskMesh);
}

void AArchaeologistDesk::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractDelegate.AddDynamic(this, &AArchaeologistDesk::OnInteract);
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
	UEquipmentComponent* EquipmentComponent = Player->GetEquipment();
	AItem* ItemInHand = EquipmentComponent->GetItemInHand();
	
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr; Artifact && !ArtifactOnDesk)
	{
		AddArtifact(Artifact, Player);
	}
	else if (ArtifactOnDesk)
	{
		ArtifactOnDesk->Clear();

		AIlluminati::SendQuestMessage<UArtifactCleanedQuestMessage>(this, [this](UArtifactCleanedQuestMessage* Message)
		{
			Message->Artifact = ArtifactOnDesk;
		});
	}
}
