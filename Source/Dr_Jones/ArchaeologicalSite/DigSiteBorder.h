// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DigSiteBorder.generated.h"

class USphereComponent;

USTRUCT(BlueprintType)
struct FDigSiteRectangularBorderDesc
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center_WS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Extents2D = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 0.0f;
};

UCLASS(Blueprintable)
class ADigSiteBorderBreakMarker : public AActor
{
	GENERATED_BODY()

public:
	ADigSiteBorderBreakMarker();

	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY()
	TObjectPtr<USphereComponent> VisSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius = 10.0f;
};

UCLASS(Blueprintable, Abstract, ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UDigSiteBorder : public UActorComponent
{
	GENERATED_BODY()

public:
	UDigSiteBorder();

	virtual void GenerateRectangularBorder(const FDigSiteRectangularBorderDesc& BorderDesc);

	UFUNCTION(BlueprintImplementableEvent)
	void OnGenerateRectangularBorder(const FDigSiteRectangularBorderDesc& BorderDesc);
};

UCLASS(Blueprintable, ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent))
class UDigSiteBorderImpl : public UDigSiteBorder
{
	GENERATED_BODY()

public:
	virtual void GenerateRectangularBorder(const FDigSiteRectangularBorderDesc& BorderDesc) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UStaticMesh> FenceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UStaticMesh> RopeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UStaticMesh> ModularBedrockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UStaticMesh> ModularWallBedrockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DigSite")
	TObjectPtr<UStaticMesh> ParapetMesh;
};
