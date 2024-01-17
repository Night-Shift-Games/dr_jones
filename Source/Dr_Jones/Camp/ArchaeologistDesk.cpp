// Property of Night Shift Games, all rights reserved.


#include "ArchaeologistDesk.h"

#include "Utilities.h"
#include "Player/PlayerComponents/InventoryComponent.h"


AArchaeologistDesk::AArchaeologistDesk()
{
	DeskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = DeskMesh;
	PrimaryActorTick.bCanEverTick = false;
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable Component"));
	InteractableComponent->InteractDelegate.AddDynamic(this, &AArchaeologistDesk::OnInteract);
}

void AArchaeologistDesk::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArchaeologistDesk::AddArtifact(AArtifact* Artifact, ADrJonesCharacter* Player)
{
	ArtifactOnDesk = Artifact;
	Player->GetInventory()->DetachActiveItemFromHand();
	FVector SocketPlace = DeskMesh->GetSocketLocation(TEXT("ArtifactSocket"));

	double ZOffset = ArtifactOnDesk->GetMeshComponent()->GetLocalBounds().BoxExtent.Z;
	FVector Origin = ArtifactOnDesk->GetMeshComponent()->GetLocalBounds().Origin;
	ZOffset = Origin.Z - ZOffset;
	
	SocketPlace = Utilities::FindGround(*this, SocketPlace + FVector (0, 0, 20), {ArtifactOnDesk});
	ArtifactOnDesk->AttachToComponent(DeskMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ArtifactSocket"));
	ArtifactOnDesk->SetActorLocation(SocketPlace - FVector(0,0, ZOffset));
	ArtifactOnDesk->GetMeshComponent()->SetVisibility(true);
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
	UInventoryComponent* Inventory = Player->GetInventory();
	AItem* ItemInHand = Inventory->GetItemInHand();
	
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr; Artifact && !ArtifactOnDesk)
	{
		Inventory->DetachActiveItemFromHand();
		AddArtifact(Artifact, Player);
	}
	else if (ArtifactOnDesk)
	{
		ArtifactOnDesk->Clear();
	}
}
