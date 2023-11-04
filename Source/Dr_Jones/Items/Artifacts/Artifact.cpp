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
