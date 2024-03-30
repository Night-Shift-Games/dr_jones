// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArtifactDatabase.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

class ULocalMeshOctree;
class UDynamicMeshComponent;
class ADrJonesCharacter;

DECLARE_DELEGATE_OneParam(FOnArtifactAttached, AArtifact*)

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
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UFUNCTION()
	void PickUp(ADrJonesCharacter* Taker);

	UFUNCTION(BlueprintImplementableEvent)
	void OnArtifactPickedUp(APawn* Taker);

	virtual UMeshComponent* GetMeshComponent() const override { return ArtifactMeshComponent; }
	
	void SetupArtifact(const FArtifactData& ArtifactData);
	void SetupDynamicArtifact();
	void Clear();
	
	virtual void OnRemovedFromEquipment() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = 1))
	FName ArtifactID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FText ArtifactName = FText::AsCultureInvariant(TEXT(""));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FText ArtifactDescription = FText::AsCultureInvariant(TEXT(""));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	int ArtifactAge = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FName ArtifactUsage = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	FName ArtifactCulture = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	EArtifactRarity ArtifactRarity = EArtifactRarity::Common;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	EArtifactSize ArtifactSize = EArtifactSize::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact", meta = (DisplayPriority = -1))
	EArtifactWear ArtifactWear = EArtifactWear::MinimalWear;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact")
	FProceduralArtifactData ProceduralData;

	UPROPERTY(EditAnywhere, Category = "Artifact")
	TObjectPtr<UStaticMesh> ArtifactStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMaterialInstanceDynamic> ArtifactDynamicMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<ULocalMeshOctree> LocalMeshOctree;

	FOnArtifactAttached OnArtifactPickup;
	
	bool bArtifactCleared = false;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UMeshComponent> ArtifactMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artifact")
	TObjectPtr<UDynamicMeshComponent> ArtifactDynamicMesh;
};

UCLASS()
class UArtifactFactory : public UObject
{
	GENERATED_BODY()
	
public:
	static AArtifact* ConstructArtifactFromDatabase(const UObject& WorldContextObject, const FName& ArtifactID);
	static FArtifactData* PullArtifactDataFromDatabase(const FName& ArtifactID);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, TSubclassOf<AArtifact> ArtifactClass);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, const FArtifactData& ArtifactData);
};
