// Property of Night Shift Games, all rights reserved.

#include "WorldDataUtilities.h"

FCombinedCultureData UWorldDataUtilities::SampleCombinedCultureData(UWorldSpatialData* Target, FGeoLocation GeoLocation)
{
	SCOPED_NAMED_EVENT(WorldDataUtilities_SampleCombinedCultureData, FColorList::Green)

	if (!Target)
	{
		UE_LOG(LogDrJones, Error, TEXT("Target was null."));
		return {};
	}

	const TSharedPtr<const FSpatialDataBuffer> Buffer = Target->GetBuffer();
	check(Buffer);

	const TSharedPtr<const FSpatialDataBufferLayout> Layout = Buffer->GetLayout();
	const TArray<FSpatialDataTexelAttributeDescriptor>& Attributes = Layout->GetAttributes();

	if (Attributes.IsEmpty())
	{
		UE_LOG(LogDrJones, Error, TEXT("There are no attributes to sample"));
		return {};
	}

	const FVector2D NormalizedCoords = GeoLocation.ConvertToNormalizedEquirectangular();

	// NOTE: 1.0f - Y
	// Spatial data Y is stored top (0.0) to bottom (1.0),
	// whereas geo location is highest at the top (north is positive)
	const FSpatialDataTexelAccessor Texel = Buffer->SampleNormalized3D(NormalizedCoords.X, 1.0f - NormalizedCoords.Y, 0.0f);

	struct FCultureTempData
	{
		TArray<TPair<int32, float>, TInlineAllocator<16>> PerYearValues;
		TArray<FCultureTimeRange, TInlineAllocator<4>> Ranges;
	};
	TMap<FName, FCultureTempData, TInlineSetAllocator<32>> CulturesDataMap;

	// Sample values in each year for each culture
	for (const FSpatialDataTexelAttributeDescriptor& Attribute : Attributes)
	{
		const uint8 RawValue = *Texel.GetAttributeData<uint8>(Attribute.Name);
		const float NormalizedValue = UWorldSpatialData::NormalizeCultureAttributeByte(RawValue);

		auto [CultureName, Year] = UWorldSpatialData::SplitWorldDataAttributeName(Attribute.Name);
		FCultureTempData& TempData = CulturesDataMap.FindOrAdd(CultureName);

		TempData.PerYearValues.Add(TPair<int32, float>(Year, NormalizedValue));
	}

	// Build time ranges for all cultures
	for (auto& [CultureName, TempData] : CulturesDataMap)
	{
		TempData.PerYearValues.Sort([](const TPair<int32, float>& A, const TPair<int32, float>& B)
		{
			return A.Key < B.Key;
		});

		int32 PreviousYear = TNumericLimits<int32>::Min();
		bool bCurrentlyInRange = false;
		for (auto It = TempData.PerYearValues.CreateConstIterator(); It; ++It)
		{
			auto& [Year, Value] = *It;

			if (Value > 0 && !bCurrentlyInRange)
			{
				bCurrentlyInRange = true;

				FCultureTimeRange Range;
				Range.Begin = PreviousYear;
				TempData.Ranges.Add(Range);
			}
			else if (bCurrentlyInRange && Value == 0)
			{
				bCurrentlyInRange = false;

				TempData.Ranges.Last().End = Year;
			}
			else if (bCurrentlyInRange && It.GetIndex() == TempData.PerYearValues.Num() - 1)
			{
				TempData.Ranges.Last().End = Year;
			}

			PreviousYear = Year;
		}

		// TODO: Maybe also add some threshold that says how much influence
		// is needed for the culture to be considered present at any given time
	}

	FCombinedCultureData CombinedCultureData;
	CombinedCultureData.Cultures.Reserve(CulturesDataMap.Num());
	for (auto& [Name, TempData] : CulturesDataMap)
	{
		FSingleCultureData SingleCultureData;
		SingleCultureData.Name = Name;
		SingleCultureData.CultureTimeRanges = TempData.Ranges;
		CombinedCultureData.Cultures.Add(SingleCultureData);
	}

	return CombinedCultureData;
}