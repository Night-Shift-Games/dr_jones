// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldSpatialData.h"

#include "WorldData.generated.h"

/** The travel destination as shown on the map */
USTRUCT(BlueprintType)
struct FTravelDestination
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Travel")
	FText DisplayName;

	/** Latitude and Longitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation")
	FGeoLocation GeoLocation;

	/** Radius of a circle shown on the map that indicates the approximate
	 *  size of the destination (roughly in kilometers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldMap", meta = (ClampMin = 0.0f))
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
class DR_JONES_API UWorldData : public UObject
{
	GENERATED_BODY()

public:
	// UWorldRegionalData();

	const TMap<FName, FTravelDestination>& GetStaticTravelDestinations() const;
	
	const UWorldSpatialData* GetCulturalInfluenceData() const;
	const UWorldSpatialData* GetGroundTypeData() const;
	
private:
	/** Available static travel destinations by key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Travel", meta = (AllowPrivateAccess = true))
	TMap<FName, FTravelDestination> StaticTravelDestinations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spatial", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWorldSpatialData> CulturalInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spatial", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWorldSpatialData> GroundType;
};

FORCEINLINE const TMap<FName, FTravelDestination>& UWorldData::GetStaticTravelDestinations() const
{
	return StaticTravelDestinations;
}

FORCEINLINE const UWorldSpatialData* UWorldData::GetCulturalInfluenceData() const
{
	return CulturalInfluence;
}

FORCEINLINE const UWorldSpatialData* UWorldData::GetGroundTypeData() const
{
	return GroundType;
}
