// Property of Night Shift Games, all rights reserved.

#pragma once

#include "ArtifactDatabase.h"
#include "CoreMinimal.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

class UInputMappingContext;
class ADrJonesCharacter;
class UDynamicMeshComponent;
class ULocalMeshOctree;

DECLARE_DELEGATE_OneParam(FOnArtifactAttached, AArtifact*)

USTRUCT(BlueprintType)
struct FArtifactDirtData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float MoldAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMaterialParameterInfo MoldMPI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float RustAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMaterialParameterInfo RustMPI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float MudAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMaterialParameterInfo MudMPI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float DustAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMaterialParameterInfo DustMPI;
};

USTRUCT(BlueprintType)
struct FProceduralArtifactData
{
	GENERATED_BODY()

	// DEPRECATED:
	// Damage blend in the material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Artifact", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float MaterialDamage = 0.0f;

	// Procedural destruction influence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Artifact", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
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

	UFUNCTION(BlueprintImplementableEvent, Category = "DrJones|Artifact")
	void OnArtifactPickedUp(APawn* Taker);

	virtual UMeshComponent* GetMeshComponent() const override;
	UDynamicMeshComponent* GetDynamicMeshComponent() const { return ArtifactDynamicMesh; }
	void SetupArtifact(const FArtifactData& ArtifactData);
	void SetupDynamicArtifact();

	void VertexPaint(const FVector& LocalPosition, const FColor& Color, const FVector4f& ChannelMask, float BrushRadiusWS = 10.0f, float BrushFalloffPow = 1.0f) const;
	void CleanCompletely() const;

	virtual void OnRemovedFromEquipment() override;

	bool IsDynamic() const;

	UFUNCTION(CallInEditor)
	static TArray<FName> GetArtifactIDs();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = 1, GetOptions = "Dr_Jones.Artifact.GetArtifactIDs"))
	FName ArtifactID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	FText ArtifactName = FText::AsCultureInvariant(TEXT(""));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	FText ArtifactDescription = FText::AsCultureInvariant(TEXT(""));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	int ArtifactAge = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	FName ArtifactUsage = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	FName ArtifactCulture = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	EArtifactRarity ArtifactRarity = EArtifactRarity::Common;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	EArtifactSize ArtifactSize = EArtifactSize::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones|Artifact", meta = (DisplayPriority = -1))
	EArtifactWear ArtifactWear = EArtifactWear::MinimalWear;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "DrJones|Artifact")
	FProceduralArtifactData ProceduralData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Artifact")
	FArtifactDirtData DirtData;

	UPROPERTY(EditAnywhere, Category = "DrJones|Artifact")
	TObjectPtr<UStaticMesh> ArtifactStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UMaterialInstanceDynamic> ArtifactDynamicMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<ULocalMeshOctree> LocalMeshOctree;

	FOnArtifactAttached OnArtifactPickup;
	
	bool bArtifactCleared = false;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UMeshComponent> ArtifactMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UDynamicMeshComponent> ArtifactDynamicMesh;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtifactCleanedDynamic, AArtifact*, Artifact);

UCLASS(Blueprintable)
class DR_JONES_API UArtifactCleaningMode : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void Begin(const ADrJonesCharacter& Character, AArtifact& Artifact);
	void End(const ADrJonesCharacter& Character);

	UFUNCTION(BlueprintCallable)
	void TickBrushStroke();

	UPROPERTY(BlueprintAssignable)
	FOnArtifactCleanedDynamic OnArtifactCleaned;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AArtifact> CurrentArtifact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(BlueprintReadOnly)
	float CleaningProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, ClampMax = 1, UIMax = 1))
	float CleaningCompletedThreshold = 0.95f;

	FVector4f CurrentPaintChannelMask;
};
