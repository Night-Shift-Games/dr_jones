// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArtifactDatabase.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

class ADrJonesCharacter;

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

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void Take(ADrJonesCharacter* Taker);

	UFUNCTION(BlueprintImplementableEvent)
	void OnArtifactTake(APawn* Taker);

	virtual UMeshComponent* GetMeshComponent() const override;

	void SetupArtifact(const FArtifactData& ArtifactData);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FName ArtifactID = NAME_None;
	FText ArtifactName = FText::AsCultureInvariant(TEXT(""));
	FText ArtifactDescription = FText::AsCultureInvariant(TEXT(""));
	int ArtifactAge = 2500.f;
	FName ArtifactUsage = NAME_None;
	FName ArtifactCulture = NAME_None;
	EArtifactRarity ArtifactRarity = EArtifactRarity::Common;
	EArtifactSize ArtifactSize = EArtifactSize::Normal;
	EArtifactWear ArtifactWear = EArtifactWear::MinimalWear;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact")
	FProceduralArtifactData ProceduralData;

	UPROPERTY(EditAnywhere, Category = "Artifact")
	TObjectPtr<UStaticMesh> ArtifactStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMaterialInstanceDynamic> ArtifactDynamicMaterial;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMeshComponent> ArtifactMeshComponent;
};

UCLASS()
class UArtifactFactory : public UObject
{
	GENERATED_BODY()
	
public:
	static AArtifact* ConstructArtifactFromDatabase(const UObject& WorldContextObject, FName& ArtifactID);
	static FArtifactData* PullArtifactDataFromDatabase(FName& ArtifactID);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, TSubclassOf<AArtifact> ArtifactClass);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, const FArtifactData& ArtifactData);
};
