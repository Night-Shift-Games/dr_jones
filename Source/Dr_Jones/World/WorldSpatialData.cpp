// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialData.h"

void UWorldSpatialData::SetBuffer(const TSharedRef<FSpatialDataBuffer>& Buffer)
{
	SpatialData = Buffer;
}
