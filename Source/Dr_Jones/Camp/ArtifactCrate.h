// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CampEntity.h"

#include "ArtifactCrate.generated.h"

class UReturnArtifactWidget;
class AArtifact;

UCLASS()
class DR_JONES_API AArtifactCrate : public ACampEntity
{
	GENERATED_BODY()

public:
	AArtifactCrate();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInteract(ADrJonesCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void AddArtifact(AArtifact* ArtifactToAdd);

	UFUNCTION(BlueprintCallable)
	AArtifact* PullOutArtifact(AArtifact* ArtifactToPullOut);
	
	UFUNCTION(BlueprintCallable)
	void SendArtifacts();

	UFUNCTION(BlueprintCallable)
	void CloseWidget();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AArtifact>> Artifacts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UReturnArtifactWidget> ReturnArtifactsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CrateStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsArchaeologistCrate = true;
};
