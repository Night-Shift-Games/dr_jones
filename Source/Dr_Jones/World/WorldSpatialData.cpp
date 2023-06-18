// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialData.h"

UWorldSpatialData::UWorldSpatialData() :
	SpatialData(FSpatialDataBuffer::Default(DefaultBufferDimensions))
{
}

void UWorldSpatialData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar << SpatialData;
}

void UWorldSpatialData::SetBuffer(const TSharedRef<FSpatialDataBuffer>& Buffer)
{
	SpatialData = Buffer;
}
