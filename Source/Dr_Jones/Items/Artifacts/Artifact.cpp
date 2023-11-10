// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "ArtifactDatabase.h"
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

	InteractableComponent->InteractDelegate.AddDynamic(this, &AArtifact::Take);
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

void AArtifact::Take(ADrJonesCharacter* Taker)
{
	OnArtifactTake(Taker);
}

UMeshComponent* AArtifact::GetMeshComponent() const
{
	return ArtifactMeshComponent;
}

void AArtifact::SetupArtifact(const FArtifactData& ArtifactData)
{
	ArtifactStaticMesh = ArtifactData.ArtifactMesh;
	
}

#if WITH_EDITOR
void AArtifact::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


AArtifact* UArtifactFactory::ConstructArtifactFromDatabase(const UObject& WorldContextObject, FName& ArtifactID)
{
	const FArtifactData* ArtifactData = PullArtifactDataFromDatabase(ArtifactID);
	return ConstructArtifact(WorldContextObject, *ArtifactData);
}

FArtifactData* UArtifactFactory::PullArtifactDataFromDatabase(FName& ArtifactID)
{
	// Get a database
	UArtifactDatabase* ArtifactDatabase = nullptr;
	if (!ArtifactDatabase)
	{
		return nullptr;
	}
	// Find object in database
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
