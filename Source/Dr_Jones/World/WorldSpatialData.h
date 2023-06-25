// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dr_Jones.h"
#include "GeoLocation.h"
#include "Spatial/SpatialData.h"
#include "UObject/Object.h"

#include "WorldSpatialData.generated.h"

/**
 * Data about the world stored as a CPU-side 3D texture
 * 
 * X,Y - GeoLocation mapped to the texture dimensions
 * Z   - The change in time of the data
 */
UCLASS(BlueprintType)
class DR_JONES_API UWorldSpatialData : public UObject
{
	GENERATED_BODY()

public:
	UWorldSpatialData();
	
	inline static const FIntVector4 DefaultBufferDimensions = { 1024, 512, 16, 1 };
	inline static const int32 DefaultBufferSize = DefaultBufferDimensions.X * DefaultBufferDimensions.Y *
		DefaultBufferDimensions.Z * DefaultBufferDimensions.W; 
	
	virtual void Serialize(FArchive& Ar) override;
	
	void SetBuffer(const TSharedRef<FSpatialDataBuffer>& Buffer);
	TSharedPtr<FSpatialDataBuffer> GetBuffer() const;

	template<typename T>
	T SampleAtLocation(const FName& AttributeName, const FGeoLocation& GeoLocation) const;

	template<typename T>
	T Sample(const FName& AttributeNameNoYear, const FGeoLocation& GeoLocation, float Year) const;

	float SampleByteNormalized(const FName& AttributeNameNoYear, const FGeoLocation& GeoLocation, float Year) const;

	TSet<FName> FindAvailableAttributeNamesExcludingYear() const;

private:
	TSharedPtr<FSpatialDataBuffer> SpatialData;
};

template <typename T>
T UWorldSpatialData::SampleAtLocation(const FName& AttributeName, const FGeoLocation& GeoLocation) const
{
	checkf(SpatialData, TEXT("Cannot sample Spatial Data. The buffer has not been set."));
	const FVector2D NormalizedCoords = GeoLocation.ConvertToNormalizedEquirectangular();
	// TODO: Use a sampler that lerps between all dimensions
	const T* Data = SpatialData->SampleNormalized3D(NormalizedCoords.X, 1.0f - NormalizedCoords.Y, 0.0f).GetAttributeData<T>(AttributeName);
	if (!Data)
	{
		UE_LOG(LogDrJones, Error, TEXT("Data at the specified location is invalid."));
		return T();
	}
	
	return *Data;
}

template <typename T>
T UWorldSpatialData::Sample(const FName& AttributeNameNoYear, const FGeoLocation& GeoLocation, float Year) const
{
	checkf(SpatialData, TEXT("Cannot sample Spatial Data. The buffer has not been set."));
	
	const FVector2D NormalizedCoords = GeoLocation.ConvertToNormalizedEquirectangular();

	// NOTE: 1.0f - Y
	// Spatial data Y is stored top (0.0) to bottom (1.0),
	// whereas geo location is highest at the top (north is positive)
	const FSpatialDataTexelAccessor Texel = SpatialData->SampleNormalized3D(NormalizedCoords.X, 1.0f - NormalizedCoords.Y, 0.0f);
	
	const TSharedPtr<const FSpatialDataBufferLayout> Layout = SpatialData->GetLayout();
	const TArray<FSpatialDataTexelAttributeDescriptor> Attributes = Layout->GetAttributes();

	if (Attributes.IsEmpty())
    {
    	UE_LOG(LogDrJones, Error, TEXT("There are no attributes to sample"));
    	return T();
    }
	
	// Interpolate year
	
	const FName* LeftAttributeName = nullptr;
	const FName* RightAttributeName = nullptr;
	int32 HighestLowerYearFound = INT_MIN;
	int32 LowestHigherYearFound = INT_MAX;
	
	for (const FSpatialDataTexelAttributeDescriptor& Attribute : Attributes)
	{
		FString AttributeName;
		FString AttributeYear;
		checkf(Attribute.Name.ToString().Split(";", &AttributeName, &AttributeYear), TEXT("Invalid attribute name has been found in WorldSpatialData (%s)"), *Attribute.Name.ToString());

		if (AttributeName != AttributeNameNoYear.ToString())
		{
			continue;
		}
		
		const int32 ConvertedYear = FCString::Atoi(*AttributeYear);
		checkf(ConvertedYear != 0 || AttributeYear == "0", TEXT("Invalid year format."));

		// Nice equal case, we can early exit
		if (ConvertedYear == Year)
		{
			return *Texel.GetAttributeData<T>(Attribute.Name);
		}

		if (ConvertedYear < Year)
		{
			if (ConvertedYear > HighestLowerYearFound)
			{
				HighestLowerYearFound = ConvertedYear;
				LeftAttributeName = &Attribute.Name;
			}
		}
		else if (ConvertedYear > Year)
		{
			if (ConvertedYear < LowestHigherYearFound)
			{
				LowestHigherYearFound = ConvertedYear;
				RightAttributeName = &Attribute.Name;
			}
		}
	}

	check(LeftAttributeName || RightAttributeName);

	// We can early out here, if we reach an outer "keyframe",
	// as there isn't another value to interpolate.
	if (!LeftAttributeName)
	{
		return *Texel.GetAttributeData<T>(*RightAttributeName);
	}
	if (!RightAttributeName)
	{
		return *Texel.GetAttributeData<T>(*LeftAttributeName);
	}
	
	const T* LeftValue = Texel.GetAttributeData<T>(*LeftAttributeName);
	const T* RightValue = Texel.GetAttributeData<T>(*RightAttributeName);

	const int32 YearDelta = LowestHigherYearFound - HighestLowerYearFound;
	const float YearDistanceFromLowest = Year - static_cast<float>(HighestLowerYearFound);
	const float Alpha = YearDistanceFromLowest / static_cast<float>(YearDelta);

	return FMath::Lerp(*LeftValue, *RightValue, Alpha);
}

FORCEINLINE TSharedPtr<FSpatialDataBuffer> UWorldSpatialData::GetBuffer() const
{
	return SpatialData;
}
