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

float UWorldSpatialData::SampleByteNormalized(const FName& AttributeNameNoYear, const FGeoLocation& GeoLocation, float Year) const
{
	const uint8 Value = Sample<uint8>(AttributeNameNoYear, GeoLocation, Year);
	return static_cast<float>(Value) / static_cast<float>(TNumericLimits<uint8>::Max());
}

TSet<FName> UWorldSpatialData::FindAvailableAttributeNamesExcludingYear() const
{
	const TSharedPtr<const FSpatialDataBufferLayout> Layout = SpatialData->GetLayout();
	const TArray<FSpatialDataTexelAttributeDescriptor>& Attributes = Layout->GetAttributes();

	TSet<FName> AvailableNames;
	
	for (const FSpatialDataTexelAttributeDescriptor& Attribute : Attributes)
	{
		FString AttributeName;
		FString AttributeYear;
		checkf(Attribute.Name.ToString().Split(";", &AttributeName, &AttributeYear), TEXT("Invalid attribute name has been found in WorldSpatialData (%s)"), *Attribute.Name.ToString());

		AvailableNames.Add(FName(AttributeName));
	}

	return AvailableNames;
}
