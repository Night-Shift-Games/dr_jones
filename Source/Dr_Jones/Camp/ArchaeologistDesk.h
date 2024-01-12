// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CampEntity.h"
#include "SharedComponents/InteractableComponent.h"
#include "ArchaeologistDesk.generated.h"

UCLASS()
class DR_JONES_API AArchaeologistDesk : public ACampEntity
{
	GENERATED_BODY()

public:
	AArchaeologistDesk();
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnInteract(ADrJonesCharacter* Player);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DeskMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ChairMesh;
};
