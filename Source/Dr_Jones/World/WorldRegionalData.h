// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpatialData.h"
#include "UObject/Object.h"

#include "WorldRegionalData.generated.h"

/** The geological location data with necessary conversion functions */
USTRUCT(BlueprintType)
struct FGeoLocation
{
	GENERATED_BODY()

	/** N/S - positive = north; negative = south */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation", meta = (ClampMin = -90.0f, ClampMax = 90.0f))
	float Latitude = 0.0f;

	/** W/E - positive = east; negative = west */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation", meta = (ClampMin = -180.0f, ClampMax = 180.0f))
	float Longitude = 0.0f;

	inline static constexpr float EquatorLength = 40075.02f;
	inline static constexpr float MeridianLength = 20003.93f;

	/** Converts the latitude and longitude to X and Y coordinates on an equirectangularly projected globe */
	FORCEINLINE FVector2D ConvertToEquirectangular() const
	{
		const float LongNormalized = (Longitude + 180.0f) / 360.0f;
		const float LatNormalized = (Latitude + 90.0f) / 180.0f; 
		const float X = LongNormalized * EquatorLength;
		const float Y = LatNormalized * MeridianLength;
		return FVector2D(X, Y);
	}

	/** Converts the X and Y coordinates on an equirectangularly projected globe to latitude and longitude */
	FORCEINLINE static FGeoLocation MakeFromEquirectangular(const FVector2D& Coords)
	{
		const float XNormalized = Coords.X / EquatorLength;
		const float YNormalized = Coords.Y / MeridianLength;
		
		FGeoLocation GeoLocation;
		GeoLocation.Longitude = XNormalized * 360.0f - 180.0f; 
		GeoLocation.Latitude = YNormalized * 180.0f - 90.0f;
		return GeoLocation;
	}
};

/** The travel destination as shown on the map */
USTRUCT(BlueprintType)
struct FTravelDestination
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel")
	FText NameOnMap;

	/** Latitude and Longitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation")
	FGeoLocation GeoLocation;

	/** Radius of a circle shown on the map that indicates the approximate
	 *  size of the destination (roughly in kilometers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldMap")
	float MapRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel", meta = (ClampMin = 0.0f))
	float BaseTravelCost = 500.0f;

	/** Whether the destination can be traveled to from the very beginning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel")
	bool bInitiallyActive = false;
};

/**
 * All the info about all the world the game takes place in.
 * This includes information about regions, cultures, missions/quests, etc.
 */
UCLASS(BlueprintType, HideCategories = (Object))
class DR_JONES_API UWorldRegionalData : public UObject
{
	GENERATED_BODY()

public:
	// UWorldRegionalData();
	
private:
	/** Available static travel destinations by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Travel", meta = (AllowPrivateAccess = true))
	TMap<FName, FTravelDestination> StaticTravelDestinations;
};
