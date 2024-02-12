// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

struct FChunk;
class AIlluminati;
class ATerrain;
class ADigSite;

USTRUCT(BlueprintType)
struct FGeoData
{
	GENERATED_BODY()
	
	FGeoData(const double InLongitude = 0.0, const double InLatitude = 0.0)
		: Longitude(InLongitude),
		  Latitude(InLatitude)
	{
	}

	FGeoData(FVector2D Coords) : Longitude(Coords.Y), Latitude(Coords.X)
	{
		
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Longitude = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Latitude = 0.0;
};

USTRUCT(BlueprintType)
struct FArchaeologicalSiteFactoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D GeoData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName = TEXT("L_ValleyOfKings");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SiteName = TEXT("Default Name");
};

UCLASS()
class DR_JONES_API UArchaeologicalSite : public UObject
{
	GENERATED_BODY()
	
public:
	UArchaeologicalSite() = default;
	UArchaeologicalSite(AIlluminati& Owner, const FName InSiteName, const FGeoData InGeoData, const FName InLevelName = TEXT("DefaultName"))
		: GeoData(InGeoData),
		  LevelName(InLevelName),
		  SiteName(InSiteName),
		  OwningIlluminati(&Owner)
	{
	}
	
	UPROPERTY(BlueprintReadWrite)
	FGeoData GeoData;
	
	UPROPERTY(BlueprintReadWrite)
	FName LevelName = TEXT("L_ValleyOfKings");

	UPROPERTY(BlueprintReadWrite)
	FName SiteName = TEXT("Default Name");
	
	TObjectPtr<AIlluminati> OwningIlluminati;
	TObjectPtr<ATerrain> Terrain;
	TObjectPtr<ADigSite> DigSite;
};

UCLASS()
class ATerrain : public AActor
{
	GENERATED_BODY()
	
};

