﻿#pragma once

#include "IDetailCustomization.h"

class UWorldSpatialData;

class FWorldSpatialDataDetailCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	explicit FWorldSpatialDataDetailCustomization();
};
