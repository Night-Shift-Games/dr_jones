// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldRegionalData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldRegionalDataUtilities.generated.h"

UCLASS()
class DR_JONES_API UWorldRegionalDataUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Converts the latitude and longitude to X and Y coordinates on an equirectangularly projected globe */
	UFUNCTION(BlueprintPure, Category = "GeoLocation|Conversion", meta = (CompactNodeTitle = "GeoToEqu"))
	static FORCEINLINE FVector2D ConvertGeoToEquirectangular(FGeoLocation GeoLocation)
	{
		return GeoLocation.ConvertToEquirectangular();
	}

	/** Converts the X and Y coordinates on an equirectangularly projected globe to latitude and longitude */
	UFUNCTION(BlueprintPure, Category = "GeoLocation|Conversion", meta = (CompactNodeTitle = "EquToGeo"))
	static FORCEINLINE FGeoLocation ConvertEquirectangularToGeo(FVector2D Coords)
	{
		return FGeoLocation::MakeFromEquirectangular(Coords);
	}

	UFUNCTION(BlueprintPure, Category = "GeoLocation", meta = (CompactNodeTitle = "Equator"))
	static FORCEINLINE float GetEquatorLength()
	{
		return FGeoLocation::EquatorLength;
	}
	
	UFUNCTION(BlueprintPure, Category = "GeoLocation", meta = (CompactNodeTitle = "Meridian"))
	static FORCEINLINE float GetMeridianLength()
	{
		return FGeoLocation::MeridianLength;
	}
};
