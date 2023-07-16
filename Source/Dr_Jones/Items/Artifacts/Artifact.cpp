// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "Player/DrJonesCharacter.h"
#include "Items/Item.h"


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

void AArtifact::Interact(APawn* Indicator)
{
	Take(Indicator);
}

void AArtifact::Take(APawn* Indicator)
{
	OnArtifactTake(Indicator);
}

UMeshComponent* AArtifact::GetMeshComponent() const
{
	return ArtifactMeshComponent;
}

#if WITH_EDITOR
void AArtifact::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
