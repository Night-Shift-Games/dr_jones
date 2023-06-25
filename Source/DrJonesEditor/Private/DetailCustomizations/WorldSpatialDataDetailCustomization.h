#pragma once

#include "IDetailCustomization.h"
#include "World/GeoLocation.h"

class UWorldSpatialData;

class FWorldSpatialDataDetailCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	explicit FWorldSpatialDataDetailCustomization();

private:
	TWeakObjectPtr<UWorldSpatialData> WorldSpatialDataPtr;
	FGeoLocation VisualizationGeoLocation;
	float VisualizationYear;
};
