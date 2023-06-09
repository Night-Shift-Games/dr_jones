// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldSpatialData.h"

#include "WorldData.generated.h"

/** Information about a specific location (might be inside a certain City) that can be visited */
USTRUCT(BlueprintType)
struct FWorldLocation
{
	GENERATED_BODY()

	/** The name displayed in the menus and texts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel")
	FText DisplayName;

	/** Latitude and Longitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation", meta = (ShowOnlyInnerProperties))
	FGeoLocation GeoLocation;

	/** Radius of a circle shown on the map that indicates the approximate
	 *  size of the location (roughly in kilometers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldMap", meta = (ClampMin = 0.0f))
	float MapRadius = 100.0f;

	/** Whether the location is available from the very beginning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel")
	bool bInitiallyActive = false;
};

USTRUCT(BlueprintType)
struct FWorldAirportInfo
{
	GENERATED_BODY()
	
	/** The name displayed in the menus and texts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/** The lowest price the player needs to pay to travel to this airport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnabled", ClampMin = 0.0f))
	float BaseTravelCost = 500.0f;
};

USTRUCT(BlueprintType)
struct FWorldHarbourInfo
{
	GENERATED_BODY()

	/** The name displayed in the menus and texts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	
	/** The lowest price the player needs to pay to travel to this harbour */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnabled", ClampMin = 0.0f))
	float BaseTravelCost = 500.0f;
};

/** City definition */
USTRUCT(BlueprintType)
struct FWorldCity
{
	GENERATED_BODY()
	
	/** The name displayed in the menus and texts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City")
	FText DisplayName;

	/** Latitude and Longitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City", meta = (ShowOnlyInnerProperties))
	FGeoLocation GeoLocation;

	/** Radius of a circle shown on the map that indicates the approximate
	 *  size of the destination (roughly in kilometers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldMap", meta = (ClampMin = 0.0f))
	float MapRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City", meta = (EditCondition = "bHasAirport"))
	FWorldAirportInfo Airport;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City", meta = (EditCondition = "bHasHarbour"))
	FWorldHarbourInfo Harbour;

	/** Controls whether the city has an active airport and can be traveled to by air transport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City", meta = (InlineEditConditionToggle))
	bool bHasAirport = false;

	/** Controls whether the city has an active harbour and can be traveled to by waterborne transport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City", meta = (InlineEditConditionToggle))
	bool bHasHarbour = false;
};

/**
 * All the info about all the world the game takes place in.
 * This includes information about regions, cultures, missions/quests, etc.
 */
UCLASS(BlueprintType, HideCategories = (Object))
class DR_JONES_API UWorldData : public UObject
{
	GENERATED_BODY()

public:
	// UWorldRegionalData();

	const TMap<FName, FWorldLocation>& GetStaticLocations() const;
	const TMap<FName, FWorldCity>& GetCities() const;
	
	const UWorldSpatialData* GetCulturalInfluenceData() const;
	const UWorldSpatialData* GetGroundTypeData() const;
	
private:
	/** Available static travel destinations by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Travel", meta = (AllowPrivateAccess = true))
	TMap<FName, FWorldLocation> StaticLocations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Travel", meta = (AllowPrivateAccess = true))
	TMap<FName, FWorldCity> Cities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spatial", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWorldSpatialData> CulturalInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spatial", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWorldSpatialData> GroundType;
};

FORCEINLINE const TMap<FName, FWorldLocation>& UWorldData::GetStaticLocations() const
{
	return StaticLocations;
}

FORCEINLINE const TMap<FName, FWorldCity>& UWorldData::GetCities() const
{
	return Cities;
}

FORCEINLINE const UWorldSpatialData* UWorldData::GetCulturalInfluenceData() const
{
	return CulturalInfluence;
}

FORCEINLINE const UWorldSpatialData* UWorldData::GetGroundTypeData() const
{
	return GroundType;
}
