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

private:
	TSharedPtr<FSpatialDataBuffer> SpatialData;
};

template <typename T>
T UWorldSpatialData::SampleAtLocation(const FName& AttributeName, const FGeoLocation& GeoLocation) const
{
	checkf(SpatialData, TEXT("Cannot sample Spatial Data. The buffer has not been set."));
	const FVector2D NormalizedCoords = GeoLocation.ConvertToNormalizedEquirectangular();
	// TODO: Use a sampler that lerps between all dimensions
	const T* Data = SpatialData->SampleNormalized3D(NormalizedCoords.X, NormalizedCoords.Y, 0.0f).GetAttributeData<T>(AttributeName);
	if (!Data)
	{
		UE_LOG(LogDrJones, Error, TEXT("Data at the specified location is invalid."));
		return T();
	}
	
	return *Data;
}

FORCEINLINE TSharedPtr<FSpatialDataBuffer> UWorldSpatialData::GetBuffer() const
{
	return SpatialData;
}
