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
	return NormalizeCultureAttributeByte(Value);
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

float UWorldSpatialData::NormalizeCultureAttributeByte(uint8 Value)
{
	return static_cast<float>(Value) / static_cast<float>(TNumericLimits<uint8>::Max());
}

TPair<FName, int32> UWorldSpatialData::SplitWorldDataAttributeName(const FName& AttributeRawName)
{
	FString AttributeName;
	FString AttributeYear;
	verifyf(AttributeRawName.ToString().Split(";", &AttributeName, &AttributeYear), TEXT("Attribute %s cannot be split, because it's not a valid World Data Attribute"), *AttributeRawName.ToString());

	const int32 ConvertedYear = FCString::Atoi(*AttributeYear);
	checkf(ConvertedYear != 0 || AttributeYear == "0", TEXT("Invalid year format."));

	return TPair<FName, int32>(AttributeName, ConvertedYear);
}
