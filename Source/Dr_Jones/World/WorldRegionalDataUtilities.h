// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dr_Jones.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldData.h"

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

	// USpatialData utilities

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static float SampleFloatAtLocation(UWorldSpatialData* Target, FName Attribute, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0.0f;
		}
		
		return Target->SampleAtLocation<float>(Attribute, GeoLocation);
	}

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static int32 SampleIntAtLocation(UWorldSpatialData* Target, FName Attribute, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0;
		}
		
		return Target->SampleAtLocation<int32>(Attribute, GeoLocation);
	}
};
