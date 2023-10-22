// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

struct FChunk;
class AIlluminati;
class ATerrain;
class ADigSite;

USTRUCT()
struct FGeoData
{
	GENERATED_BODY()
	
};

UCLASS()
class DR_JONES_API UArchaeologicalSite : public UObject
{
	GENERATED_BODY()
	
public:
	UArchaeologicalSite() = default;

	FGeoData GeoData;
	
protected:
	TObjectPtr<AIlluminati> OwningIlluminati;

	TObjectPtr<ATerrain> Terrain;
	TObjectPtr<ADigSite> DigSite;
	// TObjectPtr<ACamp> Camp;
};

UCLASS()
class ATerrain : public AActor
{
	GENERATED_BODY()
	
};

