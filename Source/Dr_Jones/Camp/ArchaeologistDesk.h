// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CampEntity.h"
#include "Items/Artifacts/Artifact.h"
#include "ArchaeologistDesk.generated.h"

UCLASS()
class DR_JONES_API AArchaeologistDesk : public ACampEntity
{
	GENERATED_BODY()

public:
	AArchaeologistDesk();
	virtual void BeginPlay() override;
	
	void AddArtifact(AArtifact* Artifact, ADrJonesCharacter* Player);
	void RemoveArtifact(AArtifact* Artifact);
	
	UFUNCTION()
	void OnInteract(ADrJonesCharacter* Player);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DeskMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ChairMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AArtifact> ArtifactOnDesk;
};
