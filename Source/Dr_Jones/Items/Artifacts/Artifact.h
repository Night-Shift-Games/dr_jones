// Property of Night Shift Games, all rights reserved.

#pragma once

#include "ArtifactDatabase.h"
#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

class UInputAction;
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

USTRUCT(BlueprintType)
struct FArtifactWhisperOfThePastData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform InteractableTransform;
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

	void VertexPaint(const FVector& LocalPosition, const FVector& LocalNormal, const FColor& Color, const FVector4f& ChannelMask, float BrushRadiusWS = 10.0f, float BrushFalloffPow = 1.0f) const;
	void CleanCompletely() const;

	virtual void OnRemovedFromEquipment() override;

	bool IsDynamic() const;

	UFUNCTION(BlueprintCallable, CallInEditor)
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

	UPROPERTY(BlueprintReadOnly)
	float CleaningProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Artifact")
	TArray<FArtifactWhisperOfThePastData> WhispersOfThePastData;

	UPROPERTY(EditAnywhere, Category = "DrJones|Artifact")
	TObjectPtr<UStaticMesh> ArtifactStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UMaterialInstanceDynamic> ArtifactDynamicMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<ULocalMeshOctree> LocalMeshOctree;

	FOnArtifactAttached OnArtifactPickup;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UMeshComponent> ArtifactMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Artifact")
	TObjectPtr<UDynamicMeshComponent> ArtifactDynamicMesh;
};

UCLASS(Blueprintable, Abstract, EditInlineNew)
class UArtifactInteractionMode : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int32 ArtifactInteractionMappingContextPriority = 690000;

	virtual UWorld* GetWorld() const override;

	void Begin(ADrJonesCharacter& Character, AArtifact& Artifact);
	void End(ADrJonesCharacter& Character);
	bool IsActive() const { return CurrentArtifact != nullptr; }

	virtual void Tick(float DeltaSeconds) {}
	virtual void OnBegin() {}
	virtual void OnEnd() {}

	virtual void OnBindInput(APlayerController& Controller);
	virtual void OnUnbindInput(APlayerController& Controller);

	ADrJonesCharacter* GetControllingCharacter() const { return ControllingCharacter; }
	AArtifact* GetCurrentArtifact() const { return CurrentArtifact; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<ADrJonesCharacter> ControllingCharacter;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<AArtifact> CurrentArtifact;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtifactCleanedDynamic, AArtifact*, Artifact);

UCLASS(Blueprintable)
class DR_JONES_API UArtifactCleaningMode : public UArtifactInteractionMode
{
	GENERATED_BODY()

public:
	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnBindInput(APlayerController& Controller) override;
	virtual void OnUnbindInput(APlayerController& Controller) override;

	UFUNCTION(BlueprintCallable)
	void TickBrushStroke();

	UPROPERTY(BlueprintAssignable)
	FOnArtifactCleanedDynamic OnArtifactCleaned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> BrushStrokeAction;
	int32 BrushStrokeActionBindingHandle = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, ClampMax = 1, UIMax = 1), Category = "Config")
	float CleaningCompletedThreshold = 0.92f;

	FVector4f CurrentPaintChannelMask = FVector4f::Zero();
};

enum class EArtifactIdentificationSphereType : int32
{
	WhispersOfThePast,
};

UCLASS(Blueprintable)
class UArtifactIdentificationSphereComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	EArtifactIdentificationSphereType Type = EArtifactIdentificationSphereType::WhispersOfThePast;
	int32 IndexInWOTPArray = INDEX_NONE;
	bool bVisibleOnlyInFront = true;
};

UCLASS(Blueprintable)
class DR_JONES_API UArtifactIdentificationMode : public UArtifactInteractionMode
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnBindInput(APlayerController& Controller) override;
	virtual void OnUnbindInput(APlayerController& Controller) override;

	void TryChangePointedSphere(UArtifactIdentificationSphereComponent* NewSphere);

	void OnPointedSphereChanged(UArtifactIdentificationSphereComponent* OldSphere, UArtifactIdentificationSphereComponent* NewSphere);

	UFUNCTION(BlueprintCallable)
	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(Transient)
	TObjectPtr<UArtifactIdentificationSphereComponent> PointedIdentificationSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FMaterialParameterInfo SphereMaskPositionMPI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FMaterialParameterInfo SphereMaskRadiusMPI;

	int32 InteractActionBindingHandle = INDEX_NONE;
};
