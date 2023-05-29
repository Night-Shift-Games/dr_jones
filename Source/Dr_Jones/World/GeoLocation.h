#pragma once

#include "GeoLocation.generated.h"

/** The geological location data with necessary conversion functions */
USTRUCT(BlueprintType)
struct FGeoLocation
{
	GENERATED_BODY()

	inline static constexpr float EquatorLength = 40075.02f;
	inline static constexpr float MeridianLength = 20003.93f;

	/** N/S - positive = north; negative = south */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation", meta = (ClampMin = -90.0f, ClampMax = 90.0f))
	float Latitude = 0.0f;

	/** W/E - positive = east; negative = west */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoLocation", meta = (ClampMin = -180.0f, ClampMax = 180.0f))
	float Longitude = 0.0f;

	/** Converts the latitude and longitude to normalized X and Y coordinates on an equirectangularly projected globe */
	FORCEINLINE FVector2D ConvertToNormalizedEquirectangular() const
	{
		const float LongNormalized = (Longitude + 180.0f) / 360.0f;
		const float LatNormalized = (Latitude + 90.0f) / 180.0f;
		return FVector2D(LongNormalized, LatNormalized);
	}

	/** Converts the latitude and longitude to X and Y coordinates on an equirectangularly projected globe */
	FORCEINLINE FVector2D ConvertToEquirectangular() const
	{
		const FVector2D Normalized = ConvertToNormalizedEquirectangular();
		const float X = Normalized.X * EquatorLength;
		const float Y = Normalized.Y * MeridianLength;
		return FVector2D(X, Y);
	}

	/** Converts normalized X and Y coordinates on an equirectangularly projected globe to latitude and longitude */
	FORCEINLINE static FGeoLocation MakeFromNormalizedEquirectangular(const FVector2D& NormalizedCoords)
	{
		FGeoLocation GeoLocation;
		GeoLocation.Longitude = NormalizedCoords.X * 360.0f - 180.0f; 
		GeoLocation.Latitude = NormalizedCoords.Y * 180.0f - 90.0f;
		return GeoLocation;
	}

	/** Converts X and Y coordinates on an equirectangularly projected globe to latitude and longitude */
	FORCEINLINE static FGeoLocation MakeFromEquirectangular(const FVector2D& Coords)
	{
		const float XNormalized = Coords.X / EquatorLength;
		const float YNormalized = Coords.Y / MeridianLength;
		return MakeFromNormalizedEquirectangular(FVector2D(XNormalized, YNormalized));
	}
};
