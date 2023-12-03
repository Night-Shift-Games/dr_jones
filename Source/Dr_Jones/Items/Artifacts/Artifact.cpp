// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "ArtifactDatabase.h"
#include "Player/PlayerComponents/InventoryComponent.h"
#include "SharedComponents/InteractableComponent.h"

AArtifact::AArtifact()
{
	ArtifactMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArtifactMesh"));
	ArtifactMeshComponent->SetupAttachment(RootComponent);

	if (ArtifactStaticMesh)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent);
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}
}

void AArtifact::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->InteractDelegate.AddDynamic(this, &AArtifact::PickUp);
}

void AArtifact::OnConstruction(const FTransform& Transform)
{
	UE_LOG(LogTemp, Warning, TEXT("Construction"));

	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
		ArtifactDynamicMaterial = StaticMeshComponent->CreateDynamicMaterialInstance(0);
	}
}

#if WITH_EDITOR
void AArtifact::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AArtifact, ArtifactID))
	{
		// Pull Data from Database
		const FArtifactData* ArtifactData = UArtifactFactory::PullArtifactDataFromDatabase(ArtifactID);
		if (!ArtifactData)
		{
			return;
		}
		// Set Data
		SetupArtifact(*ArtifactData);
	}
	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AArtifact::PickUp(ADrJonesCharacter* Taker)
{
	checkf(Taker, TEXT("Player is missing!"));
	if (UInventoryComponent* Inventory = Taker->GetInventory(); Inventory && Inventory->CanPickUpItem())
	{
		Inventory->AddArtifact(*this);
		OnArtifactPickedUp(Taker);
	}
}

void AArtifact::SetupArtifact(const FArtifactData& ArtifactData)
{
	ArtifactStaticMesh = ArtifactData.ArtifactMesh;
	ArtifactName = ArtifactData.Name;
	ArtifactDescription = ArtifactData.Description;
	ArtifactAge = FMath::RandRange(ArtifactData.AgeMin, ArtifactData.AgeMax);
	ArtifactUsage = ArtifactData.Usage;
	ArtifactCulture = ArtifactData.Culture;
	ArtifactSize = ArtifactData.Size;
	// TODO: Artifact Rarity should be based on Usage & Wear & Age / Wear.
	ArtifactRarity = ArtifactData.Rarity;
	// TODO: Artifact Wear should be rand based on Age.
	ArtifactWear = ArtifactData.Wear;
}

AArtifact* UArtifactFactory::ConstructArtifactFromDatabase(const UObject& WorldContextObject, const FName& ArtifactID)
{
	const FArtifactData* ArtifactData = PullArtifactDataFromDatabase(ArtifactID);
	return ConstructArtifact(WorldContextObject, *ArtifactData);
}

FArtifactData* UArtifactFactory::PullArtifactDataFromDatabase(const FName& ArtifactID)
{
	// Get a database.
	UArtifactDatabase* ArtifactDatabase = nullptr;
	if (!ArtifactDatabase)
	{
		return nullptr;
	}
	// Find object in database.
	FArtifactData* ArtifactData = ArtifactDatabase->ArtifactDataEntries.Find(ArtifactID);
	if (!ArtifactData)
	{
		UE_LOG(LogTemp, Error, TEXT("Artifact named %s not found in the Database!"), *ArtifactID.ToString());
		return nullptr;
	}
	return ArtifactData;
}

AArtifact* UArtifactFactory::ConstructArtifact(const UObject& WorldContextObject, TSubclassOf<AArtifact> ArtifactClass)
{
	// Get world
	UWorld* World = WorldContextObject.GetWorld();
	check(World);
	
	return World->SpawnActor<AArtifact>(ArtifactClass);
}

AArtifact* UArtifactFactory::ConstructArtifact(const UObject& WorldContextObject, const FArtifactData& ArtifactData)
{
	if (ArtifactData.CustomClass)
	{
		return ConstructArtifact(WorldContextObject, ArtifactData.CustomClass);
	}

	AArtifact* NewArtifact = WorldContextObject.GetWorld()->SpawnActor<AArtifact>();

	if (NewArtifact)
	{
		NewArtifact->SetupArtifact(ArtifactData);
	}
	
	return NewArtifact;
}
