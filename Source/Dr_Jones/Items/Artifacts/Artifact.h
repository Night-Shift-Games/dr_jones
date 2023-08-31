// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

USTRUCT(BlueprintType)
struct FProceduralArtifactData
{
	GENERATED_BODY()
	
	// Damage blend in the material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float MaterialDamage = 0.0f;

	// Procedural destruction influence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float GeometryDamage = 0.0f;
};

UCLASS(Blueprintable)
class DR_JONES_API AArtifact : public AItem
{
	GENERATED_BODY()

public:
	AArtifact();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Interact(APawn* Indicator);

	void Take(APawn* Taker);

	UFUNCTION(BlueprintImplementableEvent)
	void OnArtifactTake(APawn* Taker);

	virtual UMeshComponent* GetMeshComponent() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FName ArtifactID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact")
	FProceduralArtifactData ProceduralData;

	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMeshComponent> ArtifactMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Artifact")
	TObjectPtr<UStaticMesh> ArtifactStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMaterialInstanceDynamic> ArtifactDynamicMaterial;
};

