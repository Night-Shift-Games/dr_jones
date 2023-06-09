// Property of Night Shift Games, all rights reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "World/WorldSpatialData.h"

class FAssetTypeActions_WorldSpatialData : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_WorldSpatialData", "World Spatial Data"); }
	virtual FColor GetTypeColor() const override { return FColor(120, 120, 255); }
	virtual UClass* GetSupportedClass() const override { return UWorldSpatialData::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;

private:
	void ExecuteImportSpatialData(TArray<TWeakObjectPtr<UWorldSpatialData>> InSpatialDataObjects);
};
